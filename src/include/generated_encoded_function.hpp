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

typedef struct {
	size_t key_len;
	const char *key;
	size_t value_len;
	const char *value;
} map_entry;

class GeneratedEncodedFunction {
public:
	static void Decode(CSVEncoderBuffer &encoded_buffer, char *target_buffer, idx_t &target_buffer_current_position,
	                   const idx_t target_buffer_size, char *remaining_bytes_buffer, idx_t &remaining_bytes_size,
	                   EncodingFunction *encoding_function);

	static bool KeyLess(const char *a, size_t a_len, const char *b, size_t b_len);

	static const map_entry *FindEntry(const map_entry *map, size_t map_size, const char *search_key, size_t search_len);
};

// { 2, "\xFE\xFC", 3, (const char[]){ static_cast<char>(0xEE), static_cast<char>(0x8C), static_cast<char>(0x8E) } },

// static void Register(const DBConfig &config) {
//                 const Big5ToUtf generated_function;
//                 const EncodingFunction function(generated_function.name, GeneratedEncodedFunction::Decode,
//                                                 generated_function.max_bytes_per_byte,
//                                                 generated_function.lookup_bytes,
//                                                 reinterpret_cast<uintptr_t>(generated_function.big5_to_utf8),
//                                                 sizeof(generated_function.big5_to_utf8));
//                 config.RegisterEncodeFunction(function);
//             }
} // namespace duckdb_encodings

} // namespace duckdb