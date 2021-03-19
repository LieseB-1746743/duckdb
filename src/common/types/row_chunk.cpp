#include "duckdb/common/types/row_chunk.hpp"

#include <cfloat>
#include <limits.h>

namespace duckdb {

//! these are optimized and assume a particular byte order
#define BSWAP16(x) ((uint16_t)((((uint16_t)(x)&0xff00) >> 8) | (((uint16_t)(x)&0x00ff) << 8)))

#define BSWAP32(x)                                                                                                     \
	((uint32_t)((((uint32_t)(x)&0xff000000) >> 24) | (((uint32_t)(x)&0x00ff0000) >> 8) |                               \
	            (((uint32_t)(x)&0x0000ff00) << 8) | (((uint32_t)(x)&0x000000ff) << 24)))

#define BSWAP64(x)                                                                                                     \
	((uint64_t)((((uint64_t)(x)&0xff00000000000000ull) >> 56) | (((uint64_t)(x)&0x00ff000000000000ull) >> 40) |        \
	            (((uint64_t)(x)&0x0000ff0000000000ull) >> 24) | (((uint64_t)(x)&0x000000ff00000000ull) >> 8) |         \
	            (((uint64_t)(x)&0x00000000ff000000ull) << 8) | (((uint64_t)(x)&0x0000000000ff0000ull) << 24) |         \
	            (((uint64_t)(x)&0x000000000000ff00ull) << 40) | (((uint64_t)(x)&0x00000000000000ffull) << 56)))

RowChunk::RowChunk(BufferManager &buffer_manager, idx_t block_capacity, idx_t entry_size)
    : buffer_manager(buffer_manager), count(0), block_capacity(block_capacity), entry_size(entry_size) {
	int n = 1;
	//! little endian if true
	if (*(char *)&n == 1) {
		is_little_endian = true;
	} else {
		is_little_endian = false;
	}
}

RowChunk::RowChunk(RowChunk &other)
    : buffer_manager(other.buffer_manager), count(0), block_capacity(other.block_capacity),
      entry_size(other.entry_size), is_little_endian(other.is_little_endian) {
}

static uint8_t FlipSign(uint8_t key_byte) {
	return key_byte ^ 128;
}

static uint32_t EncodeFloat(float x) {
	uint64_t buff;

	//! zero
	if (x == 0) {
		buff = 0;
		buff |= (1u << 31);
		return buff;
	}
	//! infinity
	if (x > FLT_MAX) {
		return UINT_MAX;
	}
	//! -infinity
	if (x < -FLT_MAX) {
		return 0;
	}
	buff = Load<uint32_t>((const_data_ptr_t)&x);
	if ((buff & (1u << 31)) == 0) { //! +0 and positive numbers
		buff |= (1u << 31);
	} else {          //! negative numbers
		buff = ~buff; //! complement 1
	}

	return buff;
}

static uint64_t EncodeDouble(double x) {
	uint64_t buff;
	//! zero
	if (x == 0) {
		buff = 0;
		buff += (1ull << 63);
		return buff;
	}
	//! infinity
	if (x > DBL_MAX) {
		return ULLONG_MAX;
	}
	//! -infinity
	if (x < -DBL_MAX) {
		return 0;
	}
	buff = Load<uint64_t>((const_data_ptr_t)&x);
	if (buff < (1ull << 63)) { //! +0 and positive numbers
		buff += (1ull << 63);
	} else {          //! negative numbers
		buff = ~buff; //! complement 1
	}
	return buff;
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, bool value) {
	Store(value ? 1 : 0, dataptr);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, int8_t value) {
	Store<uint8_t>(value, dataptr);
	dataptr[0] = FlipSign(dataptr[0]);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, int16_t value) {
	Store<uint16_t>(is_little_endian ? BSWAP16(value) : value, dataptr);
	dataptr[0] = FlipSign(dataptr[0]);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, int32_t value) {
	Store<uint32_t>(is_little_endian ? BSWAP32(value) : value, dataptr);
	dataptr[0] = FlipSign(dataptr[0]);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, int64_t value) {
	Store<uint64_t>(is_little_endian ? BSWAP64(value) : value, dataptr);
	dataptr[0] = FlipSign(dataptr[0]);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, uint8_t value) {
	Store<uint8_t>(value, dataptr);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, uint16_t value) {
	Store<uint16_t>(is_little_endian ? BSWAP16(value) : value, dataptr);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, uint32_t value) {
	Store<uint32_t>(is_little_endian ? BSWAP32(value) : value, dataptr);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, uint64_t value) {
	Store<uint64_t>(is_little_endian ? BSWAP64(value) : value, dataptr);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, hugeint_t value) {
	EncodeData(dataptr, value.upper);
	EncodeData(dataptr + sizeof(value.upper), value.lower);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, float value) {
	uint32_t converted_value = EncodeFloat(value);
	Store<uint32_t>(is_little_endian ? BSWAP32(converted_value) : converted_value, dataptr);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, double value) {
	uint64_t converted_value = EncodeDouble(value);
	Store<uint64_t>(is_little_endian ? BSWAP64(converted_value) : converted_value, dataptr);
}

template <>
void RowChunk::EncodeData(data_ptr_t dataptr, interval_t value) {
	EncodeData(dataptr, value.months);
	dataptr += sizeof(value.months);
	EncodeData(dataptr, value.days);
	dataptr += sizeof(value.days);
	EncodeData(dataptr, value.micros);
}

void RowChunk::EncodeStringData(data_ptr_t dataptr, string_t value, idx_t prefix_len) {
	idx_t len = value.GetSize();
	memcpy(dataptr, value.GetDataUnsafe(), MinValue(len, prefix_len));
	if (len < prefix_len) {
		memset(dataptr + len, '\0', prefix_len - len);
	}
}

template <class T>
void RowChunk::TemplatedSerializeVectorSortable(VectorData &vdata, const SelectionVector &sel, idx_t add_count,
                                                data_ptr_t key_locations[], const bool desc, const bool has_null,
                                                const bool nulls_first) {
	auto source = (T *)vdata.data;
	if (has_null) {
		auto &validity = vdata.validity;
		const data_t valid = nulls_first ? 1 : 0;
		const data_t invalid = nulls_first ? 0 : 1;

		for (idx_t i = 0; i < add_count; i++) {
			auto idx = sel.get_index(i);
			auto source_idx = vdata.sel->get_index(idx);
			// write validity and according value
			if (validity.RowIsValid(source_idx)) {
				key_locations[i][0] = valid;
				EncodeData(key_locations[i] + 1, source[source_idx]);
			} else {
				key_locations[i][0] = invalid;
				memset(key_locations[i] + 1, 0, sizeof(T));
			}
			// invert bits if desc
			if (desc) {
				for (idx_t s = 0; s < sizeof(T) + 1; s++) {
					*(key_locations[i] + s) = ~*(key_locations[i] + s);
				}
			}
			key_locations[i] += sizeof(T) + 1;
		}
	} else {
		for (idx_t i = 0; i < add_count; i++) {
			auto idx = sel.get_index(i);
			auto source_idx = vdata.sel->get_index(idx);
			// write value
			EncodeData(key_locations[i], source[source_idx]);
			// invert bits if desc
			if (desc) {
				for (idx_t s = 0; s < sizeof(T); s++) {
					*(key_locations[i] + s) = ~*(key_locations[i] + s);
				}
			}
			key_locations[i] += sizeof(T);
		}
	}
}

void RowChunk::SerializeStringVectorSortable(VectorData &vdata, const SelectionVector &sel, idx_t add_count,
                                             data_ptr_t key_locations[], const bool desc, const bool has_null,
                                             const bool nulls_first, const idx_t prefix_len) {
	auto source = (string_t *)vdata.data;
	if (has_null) {
		auto &validity = vdata.validity;
		const data_t valid = nulls_first ? 1 : 0;
		const data_t invalid = nulls_first ? 0 : 1;

		for (idx_t i = 0; i < add_count; i++) {
			auto idx = sel.get_index(i);
			auto source_idx = vdata.sel->get_index(idx);
			// write validity and according value
			if (validity.RowIsValid(source_idx)) {
				key_locations[i][0] = valid;
				EncodeStringData(key_locations[i] + 1, source[source_idx], prefix_len);
			} else {
				key_locations[i][0] = invalid;
				memset(key_locations[i] + 1, '\0', prefix_len);
			}
			// invert bits if desc
			if (desc) {
				for (idx_t s = 0; s < prefix_len + 1; s++) {
					*(key_locations[i] + s) = ~*(key_locations[i] + s);
				}
			}
			key_locations[i] += prefix_len + 1;
		}
	} else {
		for (idx_t i = 0; i < add_count; i++) {
			auto idx = sel.get_index(i);
			auto source_idx = vdata.sel->get_index(idx);
			// write value
			EncodeStringData(key_locations[i], source[source_idx], prefix_len);
			// invert bits if desc
			if (desc) {
				for (idx_t s = 0; s < prefix_len; s++) {
					*(key_locations[i] + s) = ~*(key_locations[i] + s);
				}
			}
			key_locations[i] += prefix_len;
		}
	}
}

void RowChunk::SerializeVectorSortable(Vector &v, idx_t vcount, const SelectionVector &sel, idx_t ser_count,
                                       data_ptr_t key_locations[], bool desc, bool has_null, bool nulls_first,
                                       idx_t prefix_len) {
	VectorData vdata;
	v.Orrify(vcount, vdata);
	switch (v.GetType().InternalType()) {
	case PhysicalType::BOOL:
	case PhysicalType::INT8:
		TemplatedSerializeVectorSortable<int8_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::INT16:
		TemplatedSerializeVectorSortable<int16_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::INT32:
		TemplatedSerializeVectorSortable<int32_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::INT64:
		TemplatedSerializeVectorSortable<int64_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::UINT8:
		TemplatedSerializeVectorSortable<uint8_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::UINT16:
		TemplatedSerializeVectorSortable<uint16_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::UINT32:
		TemplatedSerializeVectorSortable<uint32_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::UINT64:
		TemplatedSerializeVectorSortable<uint64_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::INT128:
		TemplatedSerializeVectorSortable<hugeint_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::FLOAT:
		TemplatedSerializeVectorSortable<float>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::DOUBLE:
		TemplatedSerializeVectorSortable<double>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::HASH:
		TemplatedSerializeVectorSortable<hash_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::INTERVAL:
		TemplatedSerializeVectorSortable<interval_t>(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first);
		break;
	case PhysicalType::VARCHAR:
		SerializeStringVectorSortable(vdata, sel, ser_count, key_locations, desc, has_null, nulls_first, prefix_len);
		break;
	default:
		throw NotImplementedException("FIXME: unimplemented deserialize");
	}
}

static list_entry_t *GetListData(Vector &v) {
	if (v.GetVectorType() == VectorType::DICTIONARY_VECTOR) {
		auto &child = DictionaryVector::Child(v);
		return GetListData(child);
	}
    return FlatVector::GetData<list_entry_t>(v);
}

void RowChunk::ComputeEntrySizes(Vector &v, idx_t entry_sizes[], idx_t vcount, idx_t offset) {
	auto physical_type = v.GetType().InternalType();
	if (TypeIsConstantSize(physical_type)) {
		const auto type_size = GetTypeIdSize(physical_type);
		for (idx_t i = 0; i < vcount; i++) {
			entry_sizes[i] += type_size;
		}
		return;
	}

	VectorData vdata;
	v.Orrify(vcount, vdata);

	switch (physical_type) {
	case PhysicalType::VARCHAR: {
		auto strings = (string_t *)vdata.data;
		for (idx_t i = 0; i < vcount; i++) {
			idx_t str_idx = vdata.sel->get_index(i + offset);
			if (vdata.validity.RowIsValid(str_idx)) {
				entry_sizes[i] += string_t::PREFIX_LENGTH + strings[str_idx].GetSize();
			}
		}
		break;
	}
	case PhysicalType::STRUCT: {
		// obtain child vectors
		child_list_t<unique_ptr<Vector>> *children;
		vector<Vector> struct_vectors;
		if (v.GetVectorType() == VectorType::DICTIONARY_VECTOR) {
			auto &child = DictionaryVector::Child(v);
			auto &dict_sel = DictionaryVector::SelVector(v);
			children = &StructVector::GetEntries(child);
			for (auto &struct_child : *children) {
				Vector struct_vector;
				struct_vector.Slice(*struct_child.second, dict_sel, vcount);
				struct_vectors.push_back(move(struct_vector));
			}
		} else {
			children = &StructVector::GetEntries(v);
			for (auto &struct_child : *children) {
				Vector struct_vector;
				struct_vector.Reference(*struct_child.second);
				struct_vectors.push_back(move(struct_vector));
			}
		}
		// add struct validitymask size
		const idx_t struct_validitymask_size = (children->size() + 7) / 8;
		for (idx_t i = 0; i < vcount; i++) {
			// FIXME: don't serialize if the struct is NULL?
			entry_sizes[i] += struct_validitymask_size;
		}
		// compute size of child vectors
		for (auto &struct_vector : struct_vectors) {
			ComputeEntrySizes(struct_vector, entry_sizes, vcount, offset);
		}
		break;
	}
	case PhysicalType::LIST: {
		auto list_data = GetListData(v);
		auto &child_cc = ListVector::GetEntry(v);
		for (idx_t i = 0; i < vcount; i++) {
			idx_t idx = vdata.sel->get_index(i + offset);
			if (vdata.validity.RowIsValid(idx)) {
				auto list_entry = list_data[idx];

				// make room for list length, list nullmask, and list entry sizes (if non-constant size)
				entry_sizes[i] += sizeof(list_entry.length);
				entry_sizes[i] += (list_entry.length + 7) / 8;
				if (!TypeIsConstantSize(v.GetType().child_types()[0].second.InternalType())) {
					entry_sizes[i] += list_entry.length * sizeof(list_entry.length);
				}

				// compute size of each the elements in list_entry and sum them
				idx_t list_entry_sizes[STANDARD_VECTOR_SIZE];
				auto entry_remaining = list_entry.length;
				auto entry_offset = list_entry.offset;
				while (entry_remaining > 0) {
					// the list entry can span multiple vectors
					auto &chunk = child_cc.GetChunkForRow(entry_offset);
					auto offset_in_chunk = entry_offset % STANDARD_VECTOR_SIZE;
					auto next = MinValue(chunk.size() - offset_in_chunk, entry_remaining);

					// compute and add to the total
					std::fill_n(list_entry_sizes, next, 0);
					ComputeEntrySizes(chunk.data[0], list_entry_sizes, next, offset_in_chunk);
					for (idx_t size_idx = 0; size_idx < next; size_idx++) {
						entry_sizes[i] += list_entry_sizes[size_idx];
					}

					// update for next iteration
					entry_remaining -= next;
					entry_offset += next;
				}
			}
		}
		break;
	}
	default:
		throw NotImplementedException("Variable size payload type not implemented for sorting!");
	}
}

void RowChunk::ComputeEntrySizes(DataChunk &input, idx_t entry_sizes[], idx_t entry_size) {
	// fill array with constant portion of payload entry size
	std::fill_n(entry_sizes, input.size(), entry_size);

	// compute size of the constant portion of the payload columns
	VectorData vdata;
	for (idx_t col_idx = 0; col_idx < input.data.size(); col_idx++) {
		auto physical_type = input.data[col_idx].GetType().InternalType();
		if (TypeIsConstantSize(physical_type)) {
			continue;
		}
		ComputeEntrySizes(input.data[col_idx], entry_sizes, input.size());
	}
}

template <class T>
static void TemplatedSerializeVData(VectorData &vdata, const SelectionVector &sel, idx_t count, idx_t col_idx,
                                    data_ptr_t *key_locations, data_ptr_t *validitymask_locations, idx_t offset) {
	auto source = (T *)vdata.data;
	if (!validitymask_locations) {
		for (idx_t i = 0; i < count; i++) {
			auto idx = sel.get_index(i + offset);
			auto source_idx = vdata.sel->get_index(idx);

			auto target = (T *)key_locations[i];
			Store<T>(source[source_idx], (data_ptr_t)target);
			key_locations[i] += sizeof(T);
		}
	} else {
		auto byte_offset = col_idx / 8;
		auto offset_in_byte = col_idx % 8;
		for (idx_t i = 0; i < count; i++) {
			auto idx = sel.get_index(i + offset);
			auto source_idx = vdata.sel->get_index(idx);

			auto target = (T *)key_locations[i];
			Store<T>(source[source_idx], (data_ptr_t)target);
			key_locations[i] += sizeof(T);

			// set the validitymask
			if (!vdata.validity.RowIsValid(source_idx)) {
				*(validitymask_locations[i] + byte_offset) &= ~(1UL << offset_in_byte);
			}
		}
	}
}

void RowChunk::SerializeVectorData(VectorData &vdata, PhysicalType type, const SelectionVector &sel, idx_t ser_count,
                                   idx_t col_idx, data_ptr_t key_locations[], data_ptr_t validitymask_locations[],
                                   idx_t offset) {
	switch (type) {
	case PhysicalType::BOOL:
	case PhysicalType::INT8:
		TemplatedSerializeVData<int8_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations, offset);
		break;
	case PhysicalType::INT16:
		TemplatedSerializeVData<int16_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations, offset);
		break;
	case PhysicalType::INT32:
		TemplatedSerializeVData<int32_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations, offset);
		break;
	case PhysicalType::INT64:
		TemplatedSerializeVData<int64_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations, offset);
		break;
	case PhysicalType::UINT8:
		TemplatedSerializeVData<uint8_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations, offset);
		break;
	case PhysicalType::UINT16:
		TemplatedSerializeVData<uint16_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations,
		                                  offset);
		break;
	case PhysicalType::UINT32:
		TemplatedSerializeVData<uint32_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations,
		                                  offset);
		break;
	case PhysicalType::UINT64:
		TemplatedSerializeVData<uint64_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations,
		                                  offset);
		break;
	case PhysicalType::INT128:
		TemplatedSerializeVData<hugeint_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations,
		                                   offset);
		break;
	case PhysicalType::FLOAT:
		TemplatedSerializeVData<float>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations, offset);
		break;
	case PhysicalType::DOUBLE:
		TemplatedSerializeVData<double>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations, offset);
		break;
	case PhysicalType::HASH:
		TemplatedSerializeVData<hash_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations, offset);
		break;
	case PhysicalType::INTERVAL:
		TemplatedSerializeVData<interval_t>(vdata, sel, ser_count, col_idx, key_locations, validitymask_locations,
		                                    offset);
		break;
	case PhysicalType::VARCHAR: {
		auto strings = (string_t *)vdata.data;
		if (!validitymask_locations) {
			for (idx_t i = 0; i < ser_count; i++) {
				auto idx = sel.get_index(i + offset);
				auto source_idx = vdata.sel->get_index(idx);
                if (vdata.validity.RowIsValid(source_idx)) {
                    auto &string_entry = strings[source_idx];
                    // store string size
                    Store<uint32_t>(string_entry.GetSize(), key_locations[i]);
                    key_locations[i] += string_t::PREFIX_LENGTH;
                    // store the string
                    memcpy(key_locations[i], string_entry.GetDataUnsafe(), string_entry.GetSize());
                    key_locations[i] += string_entry.GetSize();
                }
			}
		} else {
			auto byte_offset = col_idx / 8;
			auto offset_in_byte = col_idx % 8;
			for (idx_t i = 0; i < ser_count; i++) {
				auto idx = sel.get_index(i + offset);
				auto source_idx = vdata.sel->get_index(idx);
				if (vdata.validity.RowIsValid(source_idx)) {
					auto &string_entry = strings[source_idx];
					// store string size
					Store<uint32_t>(string_entry.GetSize(), key_locations[i]);
					key_locations[i] += string_t::PREFIX_LENGTH;
					// store the string
					memcpy(key_locations[i], string_entry.GetDataUnsafe(), string_entry.GetSize());
					key_locations[i] += string_entry.GetSize();
				} else {
					// set the validitymask
					*(validitymask_locations[i] + byte_offset) &= ~(1UL << offset_in_byte);
				}
			}
		}
		break;
	}
	default:
		throw NotImplementedException("FIXME: unimplemented serialize");
	}
}

void RowChunk::SerializeVector(Vector &v, idx_t vcount, const SelectionVector &sel, idx_t ser_count, idx_t col_idx,
                               data_ptr_t key_locations[], data_ptr_t validitymask_locations[], idx_t offset) {
	VectorData vdata;
	v.Orrify(vcount, vdata);

	// nested types
	switch (v.GetType().InternalType()) {
	case PhysicalType::STRUCT: {
		child_list_t<unique_ptr<Vector>> *children;
		vector<Vector> struct_vectors;
		if (v.GetVectorType() == VectorType::DICTIONARY_VECTOR) {
			auto &child = DictionaryVector::Child(v);
			auto &dict_sel = DictionaryVector::SelVector(v);
			children = &StructVector::GetEntries(child);
			for (auto &struct_child : *children) {
				Vector struct_vector;
				struct_vector.Slice(*struct_child.second, dict_sel, vcount);
				struct_vectors.push_back(move(struct_vector));
			}
		} else {
			children = &StructVector::GetEntries(v);
			for (auto &struct_child : *children) {
				Vector struct_vector;
				struct_vector.Reference(*struct_child.second);
				struct_vectors.push_back(move(struct_vector));
			}
		}

		// the whole struct itself can be NULL
		auto byte_offset = col_idx / 8;
		auto offset_in_byte = col_idx % 8;

		// struct must have a validitymask for its fields
		const idx_t struct_validitymask_size = (children->size() + 7) / 8;
		data_ptr_t struct_key_locations[STANDARD_VECTOR_SIZE];
		for (idx_t i = 0; i < ser_count; i++) {
			// use key_locations as the validitymask, and create struct_key_locations
			memset(key_locations[i], -1, struct_validitymask_size);
			struct_key_locations[i] = key_locations[i] + struct_validitymask_size;

			// set whether the whole struct is null
			auto idx = sel.get_index(i + offset);
			auto source_idx = vdata.sel->get_index(idx);
			if (validitymask_locations && !vdata.validity.RowIsValid(source_idx)) {
				*(validitymask_locations[i] + byte_offset) &= ~(1UL << offset_in_byte);
			}
		}

		// now serialize the struct vectors
		for (idx_t i = 0; i < struct_vectors.size(); i++) {
			auto &struct_vector = struct_vectors[i];
			SerializeVector(struct_vector, vcount, sel, ser_count, i, struct_key_locations, key_locations);
		}
		break;
	}
	case PhysicalType::LIST: {
		auto list_data = GetListData(v);
		auto &child_cc = ListVector::GetEntry(v);
		for (idx_t i = 0; i < ser_count; i++) {
			idx_t idx = vdata.sel->get_index(i + offset);
			if (vdata.validity.RowIsValid(idx)) {
				auto list_entry = list_data[idx];

				// store list length
				Store<uint64_t>(list_entry.length, key_locations[i]);
				key_locations[i] += sizeof(list_entry.length);

				// make room for the validitymask
				data_ptr_t validitymask_location = key_locations[i];
				idx_t offset_in_byte = 0;
				idx_t validitymask_size = (list_entry.length + 7) / 8;
				memset(validitymask_location, -1, validitymask_size);
				key_locations[i] += validitymask_size;

				// store size of each entry (if variable size)
				auto child_type = v.GetType().child_types()[0].second.InternalType();
				data_ptr_t var_entry_size_ptr = nullptr;
				if (!TypeIsConstantSize(child_type)) {
					var_entry_size_ptr = key_locations[i];
					key_locations[i] += list_entry.length * sizeof(idx_t);
				}

				idx_t list_entry_sizes[STANDARD_VECTOR_SIZE];
				data_ptr_t list_entry_locations[STANDARD_VECTOR_SIZE];
				auto entry_remaining = list_entry.length;
				auto entry_offset = list_entry.offset;
				while (entry_remaining > 0) {
					// the list entry can span multiple vectors
					auto &chunk = child_cc.GetChunkForRow(entry_offset);
					auto offset_in_chunk = entry_offset % STANDARD_VECTOR_SIZE;
					auto next = MinValue(chunk.size() - offset_in_chunk, entry_remaining);

					// serialize validity
					VectorData list_vdata;
					chunk.data[0].Orrify(chunk.size(), list_vdata);
					for (idx_t entry_idx = 0; entry_idx < next; entry_idx++) {
						if (!list_vdata.validity.RowIsValid(offset_in_chunk + entry_idx)) {
							*(validitymask_location) &= ~(1UL << offset_in_byte);
						}
						if (++offset_in_byte == 8) {
							validitymask_location++;
							offset_in_byte = 0;
						}
					}

					// compute entry sizes and set locations where the list entries will be serialized
					std::fill_n(list_entry_sizes, STANDARD_VECTOR_SIZE, 0);
					ComputeEntrySizes(chunk.data[0], list_entry_sizes, next, offset_in_chunk);
					if (TypeIsConstantSize(child_type)) {
						// constant size list entries
						const idx_t type_size = GetTypeIdSize(child_type);
						for (idx_t entry_idx = 0; entry_idx < next; entry_idx++) {
							list_entry_locations[entry_idx] = key_locations[i];
							key_locations[i] += type_size;
						}
					} else {
						// variable size list entries
						for (idx_t entry_idx = 0; entry_idx < next; entry_idx++) {
							list_entry_locations[entry_idx] = key_locations[i];
							key_locations[i] += list_entry_sizes[entry_idx];
							Store<idx_t>(list_entry_sizes[entry_idx], var_entry_size_ptr);
							var_entry_size_ptr += sizeof(idx_t);
						}
					}

					// now serialize to the locations
					SerializeVector(chunk.data[0], chunk.size(), sel, next, 0, list_entry_locations, nullptr,
					                offset_in_chunk);

					// update for next iteration
					entry_remaining -= next;
					entry_offset += next;
				}
			}
		}
		break;
	}
	default:
		// non-nested types
		SerializeVectorData(vdata, v.GetType().InternalType(), sel, ser_count, col_idx, key_locations,
		                    validitymask_locations, offset);
	}
}

idx_t RowChunk::AppendToBlock(RowDataBlock &block, BufferHandle &handle, vector<BlockAppendEntry> &append_entries,
                              idx_t remaining, idx_t entry_sizes[]) {
	idx_t append_count = 0;
	data_ptr_t dataptr;
	if (entry_sizes) {
		// compute how many entries fit if entry size if variable
		dataptr = handle.node->buffer + block.byte_offset;
		for (idx_t i = 0; i < remaining; i++) {
			if (block.byte_offset + entry_sizes[i] > block_capacity * entry_size) {
				break;
			}
			append_count++;
			block.byte_offset += entry_sizes[i];
		}
	} else {
		append_count = MinValue<idx_t>(remaining, block.CAPACITY - block.count);
		dataptr = handle.node->buffer + block.count * entry_size;
	}
	append_entries.emplace_back(dataptr, append_count);
	block.count += append_count;
	return append_count;
}

void RowChunk::Build(idx_t added_count, data_ptr_t key_locations[], idx_t entry_sizes[]) {
	vector<unique_ptr<BufferHandle>> handles;
	vector<BlockAppendEntry> append_entries;

	// first allocate space of where to serialize the keys and payload columns
	idx_t remaining = added_count;
	{
		// first append to the last block (if any)
		lock_guard<mutex> append_lock(rc_lock);
		count += added_count;
		if (!blocks.empty()) {
			auto &last_block = blocks.back();
			if (last_block.count < last_block.CAPACITY) {
				// last block has space: pin the buffer of this block
				auto handle = buffer_manager.Pin(last_block.block);
				// now append to the block
				idx_t append_count = AppendToBlock(last_block, *handle, append_entries, remaining, entry_sizes);
				remaining -= append_count;
				handles.push_back(move(handle));
			}
		}
		while (remaining > 0) {
			// now for the remaining data, allocate new buffers to store the data and append there
			RowDataBlock new_block(buffer_manager, block_capacity, entry_size);
			auto handle = buffer_manager.Pin(new_block.block);

			// offset the entry sizes array if we have added entries already
			idx_t *offset_entry_sizes = entry_sizes ? entry_sizes + added_count - remaining : nullptr;

			idx_t append_count = AppendToBlock(new_block, *handle, append_entries, remaining, offset_entry_sizes);
			remaining -= append_count;

			blocks.push_back(move(new_block));
			handles.push_back(move(handle));
		}
	}
	// now set up the key_locations based on the append entries
	idx_t append_idx = 0;
	for (auto &append_entry : append_entries) {
		idx_t next = append_idx + append_entry.count;
		if (entry_sizes) {
			for (; append_idx < next; append_idx++) {
				key_locations[append_idx] = append_entry.baseptr;
				append_entry.baseptr += entry_sizes[append_idx];
			}
		} else {
			for (; append_idx < next; append_idx++) {
				key_locations[append_idx] = append_entry.baseptr;
				append_entry.baseptr += entry_size;
			}
		}
	}
}

template <class T>
static void TemplatedDeserializeIntoVector(Vector &v, idx_t count, idx_t col_idx, data_ptr_t *key_locations,
                                           data_ptr_t *validitymask_locations) {
	auto byte_offset = col_idx / 8;
	auto offset_in_byte = col_idx % 8;
	auto &validity = FlatVector::Validity(v);
	if (validitymask_locations) {
		// validity mask is not yet set: deserialize it
		for (idx_t i = 0; i < count; i++) {
			validity.Set(i, *(validitymask_locations[i] + byte_offset) & (1 << offset_in_byte));
		}
	}
	// now deserialize the data
	auto target = FlatVector::GetData<T>(v);
	for (idx_t i = 0; i < count; i++) {
		target[i] = Load<T>(key_locations[i]);
		key_locations[i] += sizeof(T);
	}
}

static ValidityMask &GetValidity(Vector &v) {
	switch (v.GetVectorType()) {
	case VectorType::DICTIONARY_VECTOR:
		return GetValidity(DictionaryVector::Child(v));
	case VectorType::FLAT_VECTOR:
		return FlatVector::Validity(v);
	case VectorType::CONSTANT_VECTOR:
		return ConstantVector::Validity(v);
	default:
		throw NotImplementedException("FIXME: cannot deserialize vector with this vectortype");
	}
}

void RowChunk::DeserializeIntoVector(Vector &v, const idx_t &vcount, const idx_t &col_idx, data_ptr_t key_locations[],
                                     data_ptr_t validitymask_locations[]) {
	auto type = v.GetType().InternalType();
	switch (type) {
	case PhysicalType::BOOL:
	case PhysicalType::INT8:
		TemplatedDeserializeIntoVector<int8_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::INT16:
		TemplatedDeserializeIntoVector<int16_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::INT32:
		TemplatedDeserializeIntoVector<int32_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::INT64:
		TemplatedDeserializeIntoVector<int64_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::UINT8:
		TemplatedDeserializeIntoVector<uint8_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::UINT16:
		TemplatedDeserializeIntoVector<uint16_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::UINT32:
		TemplatedDeserializeIntoVector<uint32_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::UINT64:
		TemplatedDeserializeIntoVector<uint64_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::INT128:
		TemplatedDeserializeIntoVector<hugeint_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::FLOAT:
		TemplatedDeserializeIntoVector<float>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::DOUBLE:
		TemplatedDeserializeIntoVector<double>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::HASH:
		TemplatedDeserializeIntoVector<hash_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::INTERVAL:
		TemplatedDeserializeIntoVector<interval_t>(v, vcount, col_idx, key_locations, validitymask_locations);
		break;
	case PhysicalType::VARCHAR: {
		auto &validity = FlatVector::Validity(v);
		if (validitymask_locations) {
			// validity mask is not yet set: deserialize it
			auto byte_offset = col_idx / 8;
			auto offset_in_byte = col_idx % 8;
			for (idx_t i = 0; i < vcount; i++) {
				validity.Set(i, *(validitymask_locations[i] + byte_offset) & (1 << offset_in_byte));
			}
		}
		idx_t len;
		auto target = FlatVector::GetData<string_t>(v);
		for (idx_t i = 0; i < vcount; i++) {
			if (!validity.RowIsValid(i)) {
				continue;
			}
			len = Load<uint32_t>(key_locations[i]);
			key_locations[i] += string_t::PREFIX_LENGTH;
			target[i] = StringVector::AddString(v, (const char *)key_locations[i], len);
			key_locations[i] += len;
		}
		break;
	}
	case PhysicalType::STRUCT: {
		// struct must have a validitymask for its fields
		auto &child_types = v.GetType().child_types();
		const idx_t struct_validitymask_size = (child_types.size() + 7) / 8;
		data_ptr_t struct_key_locations[STANDARD_VECTOR_SIZE];
		for (idx_t i = 0; i < vcount; i++) {
			// use key_locations as the validitymask, and create struct_key_locations
			struct_key_locations[i] = key_locations[i] + struct_validitymask_size;
		}

		if (validitymask_locations) {
			// validity mask is not yet set: deserialize it
			auto &validity = GetValidity(v);
			auto byte_offset = col_idx / 8;
			auto offset_in_byte = col_idx % 8;
			for (idx_t i = 0; i < vcount; i++) {
				validity.Set(i, *(validitymask_locations[i] + byte_offset) & (1 << offset_in_byte));
			}
		}

		// now deserialize into the struct vectors
		for (idx_t i = 0; i < child_types.size(); i++) {
			auto new_child = make_unique<Vector>(child_types[i].second);
			DeserializeIntoVector(*new_child, vcount, i, struct_key_locations, key_locations);
			StructVector::AddEntry(v, child_types[i].first, move(new_child));
		}
		break;
	}
	case PhysicalType::LIST: {
		if (validitymask_locations) {
			// validity mask is not yet set: deserialize it
			auto &validity = GetValidity(v);
			auto byte_offset = col_idx / 8;
			auto offset_in_byte = col_idx % 8;
			for (idx_t i = 0; i < vcount; i++) {
				validity.Set(i, *(validitymask_locations[i] + byte_offset) & (1 << offset_in_byte));
			}
		}
		VectorData vdata;
		v.Orrify(vcount, vdata);
		auto list_data = GetListData(v);
        data_ptr_t list_entry_locations[STANDARD_VECTOR_SIZE];
		auto child_cc = make_unique<ChunkCollection>();
		vector<LogicalType> types = {v.GetType().child_types()[0].second};

		uint64_t entry_offset = 0;
		for (idx_t i = 0; i < vcount; i++) {
			if (!vdata.validity.RowIsValid(i)) {
				continue;
			}
			// read list length
			auto entry_remaining = Load<uint64_t>(key_locations[i]);
			key_locations[i] += sizeof(uint64_t);
			// set list entry attributes
			list_data[i].length = entry_remaining;
			list_data[i].offset = entry_offset;
			// skip over the validity mask
			data_ptr_t validitymask_location = key_locations[i];
			idx_t offset_in_byte = 0;
			key_locations[i] += (entry_remaining + 7) / 8;
			// entry sizes
			data_ptr_t var_entry_size_ptr = nullptr;
			if (!TypeIsConstantSize(types[0].InternalType())) {
				var_entry_size_ptr = key_locations[i];
				key_locations[i] += entry_remaining * sizeof(idx_t);
			}

			// now read the list data
			while (entry_remaining > 0) {
				DataChunk chunk;
				chunk.Initialize(types);
				auto next = MinValue(entry_remaining, (idx_t)STANDARD_VECTOR_SIZE);

				// read validity
				auto &validity = GetValidity(chunk.data[0]);
				for (idx_t entry_idx = 0; entry_idx < next; entry_idx++) {
					validity.Set(entry_idx, *(validitymask_location) & (1 << offset_in_byte));
					if (++offset_in_byte == 8) {
						validitymask_location++;
						offset_in_byte = 0;
					}
				}

				// compute entry sizes and set locations where the list entries are
				if (TypeIsConstantSize(types[0].InternalType())) {
					// constant size list entries
					const idx_t type_size = GetTypeIdSize(types[0].InternalType());
					for (idx_t entry_idx = 0; entry_idx < next; entry_idx++) {
						list_entry_locations[entry_idx] = key_locations[i];
						key_locations[i] += type_size;
					}
				} else {
					// variable size list entries
					for (idx_t entry_idx = 0; entry_idx < next; entry_idx++) {
						list_entry_locations[entry_idx] = key_locations[i];
						key_locations[i] += Load<idx_t>(var_entry_size_ptr);
						var_entry_size_ptr += sizeof(idx_t);
					}
				}

				// now deserialize and add to chunk collection
				DeserializeIntoVector(chunk.data[0], next, 0, list_entry_locations, nullptr);
				chunk.SetCardinality(next);
				child_cc->Append(chunk);

				// update for next iteration
				entry_remaining -= next;
				entry_offset += next;
			}
		}
		ListVector::SetEntry(v, move(child_cc));
		break;
	}
	default:
		throw NotImplementedException("FIXME: unimplemented deserialize");
	}
}

} // namespace duckdb
