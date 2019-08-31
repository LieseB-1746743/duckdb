//===----------------------------------------------------------------------===//
//                         DuckDB
//
// storage/bufer/managed_buffer.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "common/common.hpp"
#include "common/buffer.hpp"
#include "storage/storage_info.hpp"

namespace duckdb {
class BufferManager;

//! Managed buffer is an arbitrarily-sized buffer that is at least of size >= BLOCK_SIZE
class ManagedBuffer : public Buffer {
public:
	ManagedBuffer(BufferManager &manager, index_t size, bool can_destroy, block_id_t id);

	//! The buffer manager this buffer belongs to
	BufferManager &manager;
	//! A pointer to the data held by the buffer
	unique_ptr<data_t[]> data;
	//! The size of the buffer in bytes
	index_t size;
	//! Whether or not the managed buffer can be freely destroyed when unpinned.
	//! - If can_destroy is true, the buffer can be destroyed when unpinned and hence be unrecoverable. After being destroyed, Pin() will return false.
	//! - If can_destroy is false, the buffer will instead be written to a temporary file on disk when unloaded from memory, and read back into memory when Pin() is called.
	bool can_destroy;
	//! The internal id of the buffer
	block_id_t id;
};

}
