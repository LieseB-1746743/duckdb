#include "duckdb/execution/operator/persistent/physical_insert.hpp"
#include "duckdb/parallel/thread_context.hpp"
#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"
#include "duckdb/common/types/column_data_collection.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"
#include "duckdb/execution/expression_executor.hpp"
#include "duckdb/storage/data_table.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/parser/parsed_data/create_table_info.hpp"
#include "duckdb/planner/expression/bound_constant_expression.hpp"
#include "duckdb/storage/table_io_manager.hpp"
#include "duckdb/transaction/local_storage.hpp"
#include "duckdb/parser/statement/insert_statement.hpp"
#include "duckdb/parser/statement/update_statement.hpp"
#include "duckdb/storage/table/scan_state.hpp"

namespace duckdb {

unordered_set<column_t> GetIndexedOnColumns(TableCatalogEntry *table) {
	unordered_set<column_t> indexed_columns;
	auto &indexes = table->storage->info->indexes.Indexes();
	for (auto &index : indexes) {
		for (auto &column_id : index->column_id_set) {
			indexed_columns.insert(column_id);
		}
	}
	return indexed_columns;
}

PhysicalInsert::PhysicalInsert(vector<LogicalType> types_p, TableCatalogEntry *table,
                               physical_index_vector_t<idx_t> column_index_map,
                               vector<unique_ptr<Expression>> bound_defaults,
                               vector<unique_ptr<Expression>> set_expressions, idx_t estimated_cardinality,
                               bool return_chunk, bool parallel, OnConflictAction action_type,
                               unique_ptr<Expression> on_conflict_condition_p,
                               unique_ptr<Expression> do_update_condition_p, vector<column_t> on_conflict_filter_p,
                               string constraint_name_p)
    : PhysicalOperator(PhysicalOperatorType::INSERT, move(types_p), estimated_cardinality),
      column_index_map(std::move(column_index_map)), insert_table(table), insert_types(table->GetTypes()),
      bound_defaults(move(bound_defaults)), return_chunk(return_chunk), parallel(parallel), action_type(action_type),
      set_expressions(move(set_expressions)), on_conflict_condition(move(on_conflict_condition_p)),
      do_update_condition(move(do_update_condition_p)), on_conflict_filter(move(on_conflict_filter_p)),
      constraint_name(move(constraint_name_p)) {

	indexed_on_columns = GetIndexedOnColumns(table);

	// Figure out which columns are indexed on, and will be excluded from a DO UPDATE set expression
	for (column_t i = 0; i < insert_types.size(); i++) {
		column_indices.push_back(i);
		if (indexed_on_columns.count(i)) {
			continue;
		}
		filtered_types.push_back(insert_types[i]);
		filtered_ids.push_back(i);
		filtered_physical_ids.push_back(PhysicalIndex(i));
	}
}

PhysicalInsert::PhysicalInsert(LogicalOperator &op, SchemaCatalogEntry *schema, unique_ptr<BoundCreateTableInfo> info_p,
                               idx_t estimated_cardinality, bool parallel)
    : PhysicalOperator(PhysicalOperatorType::CREATE_TABLE_AS, op.types, estimated_cardinality), insert_table(nullptr),
      return_chunk(false), schema(schema), info(move(info_p)), parallel(parallel),
      action_type(OnConflictAction::THROW) {
	GetInsertInfo(*info, insert_types, bound_defaults);
}

void PhysicalInsert::GetInsertInfo(const BoundCreateTableInfo &info, vector<LogicalType> &insert_types,
                                   vector<unique_ptr<Expression>> &bound_defaults) {
	auto &create_info = (CreateTableInfo &)*info.base;
	for (auto &col : create_info.columns.Physical()) {
		insert_types.push_back(col.GetType());
		bound_defaults.push_back(make_unique<BoundConstantExpression>(Value(col.GetType())));
	}
}

//===--------------------------------------------------------------------===//
// Sink
//===--------------------------------------------------------------------===//
class InsertGlobalState : public GlobalSinkState {
public:
	explicit InsertGlobalState(ClientContext &context, const vector<LogicalType> &return_types)
	    : insert_count(0), initialized(false), return_collection(context, return_types) {
	}

	mutex lock;
	TableCatalogEntry *table;
	idx_t insert_count;
	bool initialized;
	LocalAppendState append_state;
	ColumnDataCollection return_collection;
};

class InsertLocalState : public LocalSinkState {
public:
	InsertLocalState(ClientContext &context, const vector<LogicalType> &types,
	                 const vector<unique_ptr<Expression>> &bound_defaults)
	    : default_executor(context, bound_defaults) {
		insert_chunk.Initialize(Allocator::Get(context), types);
	}

	DataChunk insert_chunk;
	ExpressionExecutor default_executor;
	TableAppendState local_append_state;
	unique_ptr<RowGroupCollection> local_collection;
	OptimisticDataWriter *writer;
};

unique_ptr<GlobalSinkState> PhysicalInsert::GetGlobalSinkState(ClientContext &context) const {
	auto result = make_unique<InsertGlobalState>(context, GetTypes());
	if (info) {
		// CREATE TABLE AS
		D_ASSERT(!insert_table);
		auto &catalog = Catalog::GetCatalog(context);
		result->table = (TableCatalogEntry *)catalog.CreateTable(context, schema, info.get());
	} else {
		D_ASSERT(insert_table);
		result->table = insert_table;
	}
	return move(result);
}

unique_ptr<LocalSinkState> PhysicalInsert::GetLocalSinkState(ExecutionContext &context) const {
	return make_unique<InsertLocalState>(context.client, insert_types, bound_defaults);
}

void PhysicalInsert::ResolveDefaults(TableCatalogEntry *table, DataChunk &chunk,
                                     const physical_index_vector_t<idx_t> &column_index_map,
                                     ExpressionExecutor &default_executor, DataChunk &result) {
	chunk.Flatten();
	default_executor.SetChunk(chunk);

	result.Reset();
	result.SetCardinality(chunk);

	if (!column_index_map.empty()) {
		// columns specified by the user, use column_index_map
		for (auto &col : table->columns.Physical()) {
			auto storage_idx = col.StorageOid();
			auto mapped_index = column_index_map[col.Physical()];
			if (mapped_index == DConstants::INVALID_INDEX) {
				// insert default value
				default_executor.ExecuteExpression(storage_idx, result.data[storage_idx]);
			} else {
				// get value from child chunk
				D_ASSERT((idx_t)mapped_index < chunk.ColumnCount());
				D_ASSERT(result.data[storage_idx].GetType() == chunk.data[mapped_index].GetType());
				result.data[storage_idx].Reference(chunk.data[mapped_index]);
			}
		}
	} else {
		// no columns specified, just append directly
		for (idx_t i = 0; i < result.ColumnCount(); i++) {
			D_ASSERT(result.data[i].GetType() == chunk.data[i].GetType());
			result.data[i].Reference(chunk.data[i]);
		}
	}
}

void VerifyAllConflictsMeetCondition(ExecutionContext &context, DataChunk &conflicts,
                                     const unique_ptr<Expression> &condition) {
	if (!condition) {
		return;
	}
	ExpressionExecutor executor(context.client, *condition);
	DataChunk result;
	result.Initialize(context.client, {LogicalType::BOOLEAN});
	executor.Execute(conflicts, result);

	// Check that the condition holds true for all conflicts;
}

SinkResultType PhysicalInsert::Sink(ExecutionContext &context, GlobalSinkState &state, LocalSinkState &lstate_p,
                                    DataChunk &chunk) const {
	auto &gstate = (InsertGlobalState &)state;
	auto &lstate = (InsertLocalState &)lstate_p;

	auto table = gstate.table;
	PhysicalInsert::ResolveDefaults(table, chunk, column_index_map, lstate.default_executor, lstate.insert_chunk);

	if (!parallel) {
		if (!gstate.initialized) {
			table->storage->InitializeLocalAppend(gstate.append_state, context.client);
			gstate.initialized = true;
		}
		table->storage->LocalAppend(gstate.append_state, *table, context.client, lstate.insert_chunk);

		if (return_chunk) {
			gstate.return_collection.Append(lstate.insert_chunk);
		}
		gstate.insert_count += chunk.size();
	} else {
		D_ASSERT(!return_chunk);
		// parallel append
		if (!lstate.local_collection) {
			lock_guard<mutex> l(gstate.lock);
			auto &table_info = table->storage->info;
			auto &block_manager = TableIOManager::Get(*table->storage).GetBlockManagerForRowData();
			lstate.local_collection =
			    make_unique<RowGroupCollection>(table_info, block_manager, insert_types, MAX_ROW_ID);
			lstate.local_collection->InitializeEmpty();
			lstate.local_collection->InitializeAppend(lstate.local_append_state);
			lstate.writer = gstate.table->storage->CreateOptimisticWriter(context.client);
		}
		// TODO: check the conflict_target + condition here
		if (action_type == OnConflictAction::THROW) {
			table->storage->VerifyAppendConstraints(*table, context.client, lstate.insert_chunk, nullptr);
		} else {
			// Check whether any conflicts arise, and if they all meet the conflict_target + condition
			// If that's not the case - We throw the first error

			// We either want to do nothing, or perform an update when conflicts arise
			ConflictInfo conflict_info(lstate.insert_chunk.size(), constraint_name, on_conflict_filter);
			table->storage->VerifyAppendConstraints(*table, context.client, lstate.insert_chunk, &conflict_info);
			auto &conflicts = conflict_info.constraint_conflicts;
			if (conflicts.matches.Count() != 0) {
				DataChunk conflict_chunk; // contains only the conflicting values
				DataChunk scan_chunk;     // contains the original values, that caused the conflict
				DataChunk combined_chunk; // contains conflict_chunk + scan_chunk (wide)
				DataChunk update_chunk;   // contains only the to-update columns

				// Filter out everything but the conflicting rows
				conflict_chunk.Initialize(context.client, lstate.insert_chunk.GetTypes());
				conflict_chunk.ReferenceColumns(lstate.insert_chunk, column_indices);
				conflict_chunk.Slice(conflicts.matches.Selection(), conflicts.matches.Count());
				conflict_chunk.SetCardinality(conflicts.matches.Count());

				// todo: pass the on_conflict condition
				VerifyAllConflictsMeetCondition(context, conflict_chunk, on_conflict_condition);

				if (action_type != OnConflictAction::NOTHING &&
				    indexed_on_columns.size() != lstate.insert_chunk.ColumnCount()) {

					auto &data_table = table->storage;

					// Scan the existing table for the conflicting tuples, using the rowids
					scan_chunk.Initialize(context.client, lstate.insert_chunk.GetTypes());
					auto fetch_state = make_unique<ColumnFetchState>();
					data_table->Fetch(Transaction::GetTransaction(context.client), scan_chunk, column_indices,
					                  conflicts.row_ids, conflicts.matches.Count(), *fetch_state);

					// Splice the Input chunk and the fetched chunk together
					D_ASSERT(scan_chunk.ColumnCount() == conflict_chunk.ColumnCount());
					vector<LogicalType> combined_types;
					combined_types.reserve(insert_types.size() * 2);
					combined_types.insert(combined_types.end(), insert_types.begin(), insert_types.end());
					combined_types.insert(combined_types.end(), insert_types.begin(), insert_types.end());

					combined_chunk.Initialize(context.client, combined_types);
					combined_chunk.Reset();
					for (idx_t i = 0; i < insert_types.size(); i++) {
						idx_t col_idx = i;
						auto &other_col = conflict_chunk.data[i];
						auto &this_col = combined_chunk.data[col_idx];
						D_ASSERT(other_col.GetType() == this_col.GetType());
						this_col.Reference(other_col);
					}
					for (idx_t i = 0; i < insert_types.size(); i++) {
						idx_t col_idx = i + insert_types.size();
						auto &other_col = scan_chunk.data[i];
						auto &this_col = combined_chunk.data[col_idx];
						D_ASSERT(other_col.GetType() == this_col.GetType());
						this_col.Reference(other_col);
					}
					D_ASSERT(conflict_chunk.size() == scan_chunk.size());
					combined_chunk.SetCardinality(conflict_chunk.size());

					// Execute the SET expressions
					update_chunk.Initialize(context.client, filtered_types);
					ExpressionExecutor executor(context.client, set_expressions);
					executor.Execute(combined_chunk, update_chunk);

					// Perform the update, using the results of the SET expressions
					data_table->Update(*table, context.client, conflicts.row_ids, filtered_physical_ids, update_chunk);
				}
				// Cut out the conflicting columns from the insert chunk
				SelectionVector sel_vec(lstate.insert_chunk.size());
				idx_t new_size = SelectionVector::Inverted(conflicts.matches.Selection(), sel_vec,
				                                           conflicts.matches.Count(), lstate.insert_chunk.size());
				lstate.insert_chunk.Slice(sel_vec, new_size);
				lstate.insert_chunk.SetCardinality(new_size);
			}
		}
		auto new_row_group = lstate.local_collection->Append(lstate.insert_chunk, lstate.local_append_state);
		if (new_row_group) {
			lstate.writer->CheckFlushToDisk(*lstate.local_collection);
		}
	}

	return SinkResultType::NEED_MORE_INPUT;
}

void PhysicalInsert::Combine(ExecutionContext &context, GlobalSinkState &gstate_p, LocalSinkState &lstate_p) const {
	auto &gstate = (InsertGlobalState &)gstate_p;
	auto &lstate = (InsertLocalState &)lstate_p;
	auto &client_profiler = QueryProfiler::Get(context.client);
	context.thread.profiler.Flush(this, &lstate.default_executor, "default_executor", 1);
	client_profiler.Flush(context.thread.profiler);

	if (!parallel) {
		return;
	}
	if (!lstate.local_collection) {
		return;
	}
	// parallel append: finalize the append
	TransactionData tdata(0, 0);
	lstate.local_collection->FinalizeAppend(tdata, lstate.local_append_state);

	auto append_count = lstate.local_collection->GetTotalRows();

	if (append_count < LocalStorage::MERGE_THRESHOLD) {
		// we have few rows - append to the local storage directly
		lock_guard<mutex> lock(gstate.lock);
		gstate.insert_count += append_count;
		auto table = gstate.table;
		table->storage->InitializeLocalAppend(gstate.append_state, context.client);
		auto &transaction = Transaction::GetTransaction(context.client);
		lstate.local_collection->Scan(transaction, [&](DataChunk &insert_chunk) {
			table->storage->LocalAppend(gstate.append_state, *table, context.client, insert_chunk);
			return true;
		});
		table->storage->FinalizeLocalAppend(gstate.append_state);
	} else {
		// we have many rows - flush the row group collection to disk (if required) and merge into the transaction-local
		// state
		lstate.writer->FlushToDisk(*lstate.local_collection);
		lstate.writer->FinalFlush();

		lock_guard<mutex> lock(gstate.lock);
		gstate.insert_count += append_count;
		gstate.table->storage->LocalMerge(context.client, *lstate.local_collection);
	}
}

SinkFinalizeType PhysicalInsert::Finalize(Pipeline &pipeline, Event &event, ClientContext &context,
                                          GlobalSinkState &state) const {
	auto &gstate = (InsertGlobalState &)state;
	if (!parallel && gstate.initialized) {
		auto table = gstate.table;
		table->storage->FinalizeLocalAppend(gstate.append_state);
	}
	return SinkFinalizeType::READY;
}

//===--------------------------------------------------------------------===//
// Source
//===--------------------------------------------------------------------===//
class InsertSourceState : public GlobalSourceState {
public:
	explicit InsertSourceState(const PhysicalInsert &op) : finished(false) {
		if (op.return_chunk) {
			D_ASSERT(op.sink_state);
			auto &g = (InsertGlobalState &)*op.sink_state;
			g.return_collection.InitializeScan(scan_state);
		}
	}

	ColumnDataScanState scan_state;
	bool finished;
};

unique_ptr<GlobalSourceState> PhysicalInsert::GetGlobalSourceState(ClientContext &context) const {
	return make_unique<InsertSourceState>(*this);
}

void PhysicalInsert::GetData(ExecutionContext &context, DataChunk &chunk, GlobalSourceState &gstate,
                             LocalSourceState &lstate) const {
	auto &state = (InsertSourceState &)gstate;
	auto &insert_gstate = (InsertGlobalState &)*sink_state;
	if (state.finished) {
		return;
	}
	if (!return_chunk) {
		chunk.SetCardinality(1);
		chunk.SetValue(0, 0, Value::BIGINT(insert_gstate.insert_count));
		state.finished = true;
		return;
	}

	insert_gstate.return_collection.Scan(state.scan_state, chunk);
}

} // namespace duckdb
