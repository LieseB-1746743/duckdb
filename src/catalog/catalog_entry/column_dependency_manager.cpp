#include "duckdb/catalog/catalog_entry/column_dependency_manager.hpp"
#include "duckdb/parser/column_definition.hpp"
#include "duckdb/common/set.hpp"

namespace duckdb {

ColumnDependencyManager::ColumnDependencyManager() {
}

ColumnDependencyManager::~ColumnDependencyManager() {
}

void ColumnDependencyManager::AddGeneratedColumn(ColumnDefinition &column, const vector<column_t> &indices) {
	D_ASSERT(column.Generated());

	if (indices.empty()) {
		return;
	}
	auto index = column.oid;
	auto &list = dependents_map[index];
	for (auto &dep : indices) {
		// Add this column as a dependency of the new column
		list.insert(dep);
		// Add the new column as a dependent of the column
		dependencies_map[dep].insert(index);
		// Inherit the dependencies
		if (HasDependencies(dep)) {
			auto &inherited_deps = dependents_map[dep];
			D_ASSERT(!inherited_deps.empty());
			for (auto &inherited_dep : inherited_deps) {
				list.insert(inherited_dep);
				dependencies_map[inherited_dep].insert(index);
			}
		}
	}
}

vector<column_t> ColumnDependencyManager::RemoveColumn(column_t index, column_t column_amount) {
	// Always add the initial column
	deleted_columns.insert(index);

	RemoveGeneratedColumn(index);
	RemoveStandardColumn(index);

	// Clean up the internal list
	vector<column_t> new_indices = CleanupInternals(column_amount);
	D_ASSERT(deleted_columns.empty());
	return new_indices;
}

bool ColumnDependencyManager::IsDependencyOf(column_t gcol, column_t col) const {
	auto entry = dependents_map.find(gcol);
	if (entry == dependents_map.end()) {
		return false;
	}
	auto &list = entry->second;
	return list.count(col);
}

bool ColumnDependencyManager::HasDependencies(column_t index) const {
	auto entry = dependents_map.find(index);
	if (entry == dependents_map.end()) {
		return false;
	}
	return true;
}

const unordered_set<column_t> &ColumnDependencyManager::GetDependencies(column_t index) const {
	auto entry = dependents_map.find(index);
	D_ASSERT(entry != dependents_map.end());
	return entry->second;
}

bool ColumnDependencyManager::HasDependents(column_t index) const {
	auto entry = dependencies_map.find(index);
	if (entry == dependencies_map.end()) {
		return false;
	}
	return true;
}

const unordered_set<column_t> &ColumnDependencyManager::GetDependents(column_t index) const {
	auto entry = dependencies_map.find(index);
	D_ASSERT(entry != dependencies_map.end());
	return entry->second;
}

void ColumnDependencyManager::RemoveStandardColumn(column_t index) {
	if (!HasDependents(index)) {
		return;
	}
	auto dependents = dependencies_map[index];
	for (auto &gcol : dependents) {
		RemoveGeneratedColumn(gcol);
	}
	// Remove this column from the dependencies map
	dependencies_map.erase(index);
}

void ColumnDependencyManager::RemoveGeneratedColumn(column_t index) {
	deleted_columns.insert(index);
	if (!HasDependencies(index)) {
		return;
	}
	auto &dependencies = dependents_map[index];
	for (auto &col : dependencies) {
		// Remove this generated column from the list of this column
		auto &col_dependents = dependencies_map[col];
		D_ASSERT(col_dependents.count(index));
		col_dependents.erase(index);
		// If the resulting list is empty, remove the column from the dependencies map altogether
		if (col_dependents.empty()) {
			dependencies_map.erase(col);
		}
	}
	// Remove this column from the dependents_map map
	dependents_map.erase(index);
}

void ColumnDependencyManager::AdjustSingle(column_t idx, idx_t offset) {
	D_ASSERT(idx >= offset);
	column_t new_idx = idx - offset;
	// Adjust this index in the dependents of this column
	bool has_dependents = HasDependents(idx);
	bool has_dependencies = HasDependencies(idx);

	if (has_dependents) {
		auto &dependents = GetDependents(idx);
		for (auto &dep : dependents) {
			auto &dep_dependencies = dependents_map[dep];
			dep_dependencies.erase(idx);
			D_ASSERT(!dep_dependencies.count(new_idx));
			dep_dependencies.insert(new_idx);
		}
	}
	if (has_dependencies) {
		auto &dependencies = GetDependencies(idx);
		for (auto &dep : dependencies) {
			auto &dep_dependents = dependencies_map[dep];
			dep_dependents.erase(idx);
			D_ASSERT(!dep_dependents.count(new_idx));
			dep_dependents.insert(new_idx);
		}
	}
	if (has_dependents) {
		D_ASSERT(!dependencies_map.count(new_idx));
		dependencies_map[new_idx] = move(dependencies_map[idx]);
		dependencies_map.erase(idx);
	}
	if (has_dependencies) {
		D_ASSERT(!dependents_map.count(new_idx));
		dependents_map[new_idx] = move(dependents_map[idx]);
		dependents_map.erase(idx);
	}
}

vector<column_t> ColumnDependencyManager::CleanupInternals(column_t column_amount) {
	vector<column_t> to_adjust;
	D_ASSERT(!deleted_columns.empty());
	// Get the lowest index that was deleted
	vector<column_t> new_indices(column_amount, DConstants::INVALID_INDEX);
	column_t threshold = *deleted_columns.begin();

	idx_t offset = 0;
	for (column_t i = 0; i < column_amount; i++) {
		new_indices[i] = i - offset;
		if (deleted_columns.count(i)) {
			offset++;
			continue;
		}
		if (i > threshold && (HasDependencies(i) || HasDependents(i))) {
			to_adjust.push_back(i);
		}
	}

	// Adjust all indices inside the dependency managers internal mappings
	for (auto &col : to_adjust) {
		offset = col - new_indices[col];
		AdjustSingle(col, offset);
	}
	deleted_columns.clear();
	return new_indices;
}

} // namespace duckdb
