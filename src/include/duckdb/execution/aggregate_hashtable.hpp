//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/execution/aggregate_hashtable.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/types/row/tuple_data_collection.hpp"
#include "duckdb/execution/base_aggregate_hashtable.hpp"
#include "duckdb/storage/arena_allocator.hpp"
#include "duckdb/storage/buffer/buffer_handle.hpp"

namespace duckdb {
class BlockHandle;
class BufferHandle;

struct FlushMoveState;

//! GroupedAggregateHashTable is a linear probing HT that is used for computing
//! aggregates
/*!
    GroupedAggregateHashTable is a HT that is used for computing aggregates. It takes
   as input the set of groups and the types of the aggregates to compute and
   stores them in the HT. It uses linear probing for collision resolution.
*/

// two part hash table
// hashes and payload
// hashes layout:
// [48 BIT POINTER][16 BIT SALT]
//
// payload layout
// [VALIDITY][GROUPS][HASH][PADDING][PAYLOAD]
// [VALIDITY] is the validity bits of the data columns (including the HASH)
// [GROUPS] is the group data, could be multiple values, fixed size, strings are elsewhere
// [HASH] is the hash data of the groups
// [PADDING] is gunk data to align payload properly
// [PAYLOAD] is the payload (i.e. the aggregate states)

struct aggr_ht_entry_t {
public:
	explicit aggr_ht_entry_t(hash_t value_p) : value(value_p) {
	}

	inline bool IsOccupied() const {
		return value != 0;
	}

	inline data_ptr_t GetPointer() const {
		D_ASSERT(IsOccupied());
		return reinterpret_cast<data_ptr_t>(value & POINTER_MASK);
	}
	inline void SetPointer(data_ptr_t pointer) {
		// Pointer shouldn't use upper bits
		D_ASSERT((reinterpret_cast<uint64_t>(pointer) & SALT_MASK) == 0);
		// Value should have all 1's in the pointer area
		D_ASSERT((value & POINTER_MASK) == POINTER_MASK);
		// Set upper bits to 1 in pointer so the salt stays intact
		value &= reinterpret_cast<uint64_t>(pointer) | SALT_MASK;
	}

	static inline hash_t ExtractSalt(hash_t hash) {
		// Leaves upper bits intact, sets lower bits to all 1's
		return hash | POINTER_MASK;
	}
	inline hash_t GetSalt() const {
		return ExtractSalt(value);
	}
	inline void SetSalt(hash_t salt) {
		// Shouldn't be occupied when we set this
		D_ASSERT(!IsOccupied());
		// Salt should have all 1's in the pointer field
		D_ASSERT((salt & POINTER_MASK) == POINTER_MASK);
		// No need to mask, just put the whole thing there
		value = salt;
	}

private:
	//! Upper 16 bits are salt
	static constexpr const hash_t SALT_MASK = 0xFFFF000000000000;
	//! Lower 48 bits are the pointer
	static constexpr const hash_t POINTER_MASK = 0x0000FFFFFFFFFFFF;

	hash_t value;
};

struct AggregateHTAppendState {
	AggregateHTAppendState();

	Vector ht_offsets;
	Vector hash_salts;
	SelectionVector group_compare_vector;
	SelectionVector no_match_vector;
	SelectionVector empty_vector;
	SelectionVector new_groups;
	Vector addresses;
	unsafe_unique_array<UnifiedVectorFormat> group_data;
	DataChunk group_chunk;

	TupleDataChunkState chunk_state;
	bool chunk_state_initialized;
};

class GroupedAggregateHashTable : public BaseAggregateHashTable {
public:
	GroupedAggregateHashTable(ClientContext &context, Allocator &allocator, vector<LogicalType> group_types,
	                          vector<LogicalType> payload_types, const vector<BoundAggregateExpression *> &aggregates,
	                          idx_t initial_capacity = InitialCapacity());
	GroupedAggregateHashTable(ClientContext &context, Allocator &allocator, vector<LogicalType> group_types,
	                          vector<LogicalType> payload_types, vector<AggregateObject> aggregates,
	                          idx_t initial_capacity = InitialCapacity());
	GroupedAggregateHashTable(ClientContext &context, Allocator &allocator, vector<LogicalType> group_types);
	~GroupedAggregateHashTable() override;

public:
	//! Get the data collection of this HT
	TupleDataCollection &GetDataCollection();
	//! Number of groups in the HT
	idx_t Count() const;
	//! Initial capacity of the HT
	static idx_t InitialCapacity();
	//! Max capacity while sinking data into the HT
	static idx_t SinkCapacity();
	//! Current capacity of the HT
	idx_t Capacity() const;

	//! The size (in bytes) of the data collection
	idx_t DataSize() const;
	//! The size (in bytes) of the first part of the HT
	static idx_t FirstPartSize(idx_t count);
	//! The total size (first part and data) of the HT
	idx_t TotalSize() const;

	//! Add the given data to the HT, computing the aggregates grouped by the
	//! data in the group chunk. When resize = true, aggregates will not be
	//! computed but instead just assigned.
	idx_t AddChunk(AggregateHTAppendState &state, DataChunk &groups, DataChunk &payload,
	               const unsafe_vector<idx_t> &filter);
	idx_t AddChunk(AggregateHTAppendState &state, DataChunk &groups, Vector &group_hashes, DataChunk &payload,
	               const unsafe_vector<idx_t> &filter);
	idx_t AddChunk(AggregateHTAppendState &state, DataChunk &groups, DataChunk &payload, AggregateType filter);

	//! Scan the HT starting from the scan_position until the result and group
	//! chunks are filled. scan_position will be updated by this function.
	//! Returns the amount of elements found.
	idx_t Scan(TupleDataParallelScanState &gstate, TupleDataLocalScanState &lstate, DataChunk &result);

	//! Fetch the aggregates for specific groups from the HT and place them in the result
	void FetchAggregates(DataChunk &groups, DataChunk &result);

	//! Finds or creates groups in the hashtable using the specified group keys. The addresses vector will be filled
	//! with pointers to the groups in the hash table, and the new_groups selection vector will point to the newly
	//! created groups. The return value is the amount of newly created groups.
	idx_t FindOrCreateGroups(AggregateHTAppendState &state, DataChunk &groups, Vector &group_hashes,
	                         Vector &addresses_out, SelectionVector &new_groups_out);
	idx_t FindOrCreateGroups(AggregateHTAppendState &state, DataChunk &groups, Vector &addresses_out,
	                         SelectionVector &new_groups_out);
	void FindOrCreateGroups(AggregateHTAppendState &state, DataChunk &groups, Vector &addresses_out);

	//! Executes the filter(if any) and update the aggregates
	void Combine(GroupedAggregateHashTable &other);
	void Combine(TupleDataCollection &other_data);

	//! Appends the data in the other HT to this one
	void Append(GroupedAggregateHashTable &other);

	//! Radix partition the HT into multiple HTs
	void Partition(vector<GroupedAggregateHashTable *> &partition_hts, idx_t radix_bits, bool sink_done);

	//! Unpin the first part and data blocks
	void Finalize();

private:
	//! The hash table load factor, when a resize is triggered
	constexpr static float LOAD_FACTOR = 1.5;
	//! The size of the first part of the HT, should fit in L2 cache
	constexpr static idx_t FIRST_PART_SINK_SIZE = 1048576;

	//! The data of the HT
	unique_ptr<TupleDataCollection> data_collection;
	//! Block pin state of the HT (for appending data)
	TupleDataPinState td_pin_state;
	//! Predicates for matching groups (always ExpressionType::COMPARE_EQUAL)
	vector<ExpressionType> predicates;

	//! The capacity of the HT. This can be increased using GroupedAggregateHashTable::Resize
	idx_t capacity;
	//! The hash map (first part) of the HT: allocated data and pointer into it
	AllocatedData hash_map;
	aggr_ht_entry_t *entries;
	//! Offset of the hash column in the rows
	idx_t hash_offset;
	//! Bitmask for getting relevant bits from the hashes to determine the position
	hash_t bitmask;

	//! The active arena allocator used by the aggregates for their internal state
	shared_ptr<ArenaAllocator> aggregate_allocator;
	//! Owning arena allocators that this HT has data from
	vector<shared_ptr<ArenaAllocator>> stored_allocators;

	//! Whether this HT has been finalized (first part and data blocks have been unpinned)
	bool is_finalized;

private:
	//! Disabled the copy constructor
	GroupedAggregateHashTable(const GroupedAggregateHashTable &) = delete;
	//! Destroy the HT
	void Destroy();

	//! Apply bitmask to get the entry in the HT
	inline idx_t ApplyBitMask(hash_t hash) const;
	//! Threshold at which to resize the HT
	idx_t ResizeThreshold() const;
	//! Resize the HT to the specified size. Must be larger than the current size.
	void Resize(idx_t size);
	//! Initializes the hashes of the HT
	void InitializeFirstPart();

	//! Does the actual group matching / creation
	idx_t FindOrCreateGroupsInternal(AggregateHTAppendState &state, DataChunk &groups, Vector &group_hashes,
	                                 Vector &addresses, SelectionVector &new_groups);

	//! Verify the first part of the HT
	void Verify();
};

} // namespace duckdb
