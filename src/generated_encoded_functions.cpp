#include "generated_encoded_function.hpp"

#include "duckdb/execution/operator/csv_scanner/encode/csv_encoder.hpp"

namespace duckdb {
namespace duckdb_encodings {

GeneratedEncodedFunction::GeneratedEncodedFunction(const string &encode_name, const idx_t bytes_per_iteration,
                                                   const idx_t lookup_bytes,
                                                   const map<vector<uint8_t>, vector<uint8_t>> &conversion_map)
    : EncodingFunction(encode_name, Decode, bytes_per_iteration, lookup_bytes), conversion_map(conversion_map) {};

void GeneratedEncodedFunction::Decode(CSVEncoderBuffer &encoded_buffer, char *target_buffer,
                                      idx_t &target_buffer_current_position, const idx_t target_buffer_size,
                                      char *remaining_bytes_buffer, idx_t &remaining_bytes_size,
                                      EncodingFunction *encoding_function) {
	auto &generated_function = *static_cast<GeneratedEncodedFunction *>(encoding_function);
	auto encoded_buffer_ptr = encoded_buffer.Ptr();
	while (encoded_buffer.cur_pos < encoded_buffer.actual_encoded_buffer_size) {
		// We need to use our map from the highest to lowest lookup bytes
		if (encoded_buffer.actual_encoded_buffer_size - encoded_buffer.cur_pos < generated_function.lookup_bytes &&
		    !encoded_buffer.last_buffer) {
			// Not enough bytes to check.
			return;
		}
		int byte_group = static_cast<int>(generated_function.lookup_bytes);
		if (encoded_buffer.actual_encoded_buffer_size - encoded_buffer.cur_pos < generated_function.lookup_bytes) {
			byte_group = static_cast<int>(encoded_buffer.actual_encoded_buffer_size - encoded_buffer.cur_pos);
		}
		for (; byte_group > 0; --byte_group) {
			vector<uint8_t> bytes;
			for (idx_t byte_pos = 0; byte_pos < byte_group; ++byte_pos) {
				bytes.push_back(encoded_buffer_ptr[encoded_buffer.cur_pos + byte_pos]);
			}
			// We found a match, do the conversion
			if (generated_function.conversion_map.find(bytes) != generated_function.conversion_map.end()) {
				// We walk the byte group size from our buffer source
				encoded_buffer.cur_pos += byte_group;
				vector<uint8_t> converted_bytes = generated_function.conversion_map[bytes];
				for (idx_t byte_pos = 0; byte_pos < converted_bytes.size(); ++byte_pos) {
					if (target_buffer_current_position == target_buffer_size) {
						// We are done, but we have to store one byte for the next chunk!
						remaining_bytes_size = converted_bytes.size() - byte_pos;
						for (idx_t remaining_pos = 0; remaining_pos < remaining_bytes_size; ++remaining_pos) {
							remaining_bytes_buffer[remaining_pos] =
							    static_cast<char>(converted_bytes[byte_pos + remaining_pos]);
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
} // namespace duckdb_encodings

} // namespace duckdb