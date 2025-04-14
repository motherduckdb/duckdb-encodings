//===----------------------------------------------------------------------===//
//
//                         DuckDB - Encodings
//
// generated/generated_encoded_functions.hpp
//
//
//===----------------------------------------------------------------------===//

#include "encoding_maps.hpp"
#include "duckdb/main/config.hpp"
#include "duckdb/function/encoding_function.hpp"

namespace duckdb {
namespace duckdb_encodings {

class GeneratedEncodedFunction: public EncodingFunction {
	public:
	GeneratedEncodedFunction(const string &encode_name,
	                 const idx_t bytes_per_iteration, const idx_t lookup_bytes): EncodingFunction(encode_name,Decode, bytes_per_iteration, lookup_bytes) {};

	// fix-me how to initialize the map?


	//! A convertion map, that indicates byte replacements.
	map<vector<uint8_t>, vector<uint8_t>> conversion_map;

	static void Decode(const char *source_buffer, idx_t &source_buffer_current_position, const idx_t source_buffer_size,
                       char *target_buffer, idx_t &target_buffer_current_position, const idx_t target_buffer_size,
                       char *remaining_bytes_buffer, idx_t &remaining_bytes_size, EncodingFunction* encoding_function) {
		auto &generated_function = *static_cast<GeneratedEncodedFunction*>(encoding_function);
		// TODO what if it is the last buffer
		while (source_buffer_current_position < source_buffer_size){
			// We need to use our map from the highest to lowest lookup bytes
			for (int byte_group = static_cast<int>(generated_function.lookup_bytes); byte_group > 0; --byte_group) {
				vector<uint8_t> bytes;
				for (idx_t byte_pos = 0; byte_pos < byte_group; ++byte_pos){
					bytes.push_back(source_buffer[source_buffer_current_position+byte_pos]);
				}
				// We found a match, do the conversion
				if (generated_function.conversion_map.find(bytes) != generated_function.conversion_map.end()) {
					// We walk the byte group size from our buffer source
					source_buffer_current_position += byte_group;
					vector<uint8_t> converted_bytes = generated_function.conversion_map[bytes];
					for (idx_t byte_pos = 0; byte_pos < converted_bytes.size(); ++byte_pos) {
						if (target_buffer_current_position == target_buffer_size) {
							// We are done, but we have to store one byte for the next chunk!
							remaining_bytes_size = converted_bytes.size() - byte_pos;
							for (idx_t remaining_pos = 0; remaining_pos < remaining_bytes_size; ++remaining_pos) {
								remaining_bytes_buffer[remaining_pos] = static_cast<char>(converted_bytes[byte_pos + remaining_pos]);
							}
							return;
						}
						target_buffer[target_buffer_current_position++] == converted_bytes[byte_pos];
					}
					// We finished off this group
					break;
				}
			}
		}
}

};

    } // namespace duckdb_encodings

    } // namespace duckdb