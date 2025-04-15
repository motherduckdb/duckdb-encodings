//===----------------------------------------------------------------------===//
//
//                         DuckDB - Encodings
//
// generated_encoded_function.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/main/config.hpp"
#include "duckdb/function/encoding_function.hpp"

namespace duckdb {
namespace duckdb_encodings {

class GeneratedEncodedFunction : public EncodingFunction {
public:
	GeneratedEncodedFunction(const string &encode_name, const idx_t bytes_per_iteration, const idx_t lookup_bytes,
	                         const map<vector<uint8_t>, vector<uint8_t>> &conversion_map);

	//! A convertion map, that indicates byte replacements.
	map<vector<uint8_t>, vector<uint8_t>> conversion_map;

	static void Decode(CSVEncoderBuffer &encoded_buffer, char *target_buffer, idx_t &target_buffer_current_position,
	                   const idx_t target_buffer_size, char *remaining_bytes_buffer, idx_t &remaining_bytes_size,
	                   EncodingFunction *encoding_function);
};

} // namespace duckdb_encodings

} // namespace duckdb