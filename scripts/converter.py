import sys
import os
import re

def unicode_to_utf8_bytes(codepoint: int) -> list[int]:
    """Convert a Unicode codepoint to a list of UTF-8 bytes."""
    return list(chr(codepoint).encode("utf-8"))

def parse_ucm(file_path: str):
    encoding_name = None
    bytemap = {}
    max_utf8_len = 0

    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    in_charmap = False
    for line in lines:
        line = line.strip()
        if not line or line.startswith('#'):
            if line.startswith('# Encoding name:'):
                encoding_name = line.split()[-1].strip('"')
            continue

        

        if line == "CHARMAP":
            in_charmap = True
            continue
        if line == "END CHARMAP":
            break

        if in_charmap:
            match = re.match(r"<U([0-9A-Fa-f]+)>\s+((?:\\x[0-9A-Fa-f]{2})+)\s+\|[0-3]", line)
            if match:
                codepoint = int(match.group(1), 16)
                utf8_bytes = unicode_to_utf8_bytes(codepoint)
                max_utf8_len = max(max_utf8_len, len(utf8_bytes))

                code_bytes = bytes(int(b[2:], 16) for b in re.findall(r'\\x[0-9A-Fa-f]{2}', match.group(2)))
                bytemap[code_bytes] = utf8_bytes

    if not encoding_name:
        raise ValueError("Encoding name not found in UCM file")

    return encoding_name.replace("-", "_"), bytemap, max_utf8_len

def generate_cpp_map(encoding_name: str, bytemap: dict, max_utf8_len: int, filepath: str):
    # Determine map size
    max_code_bytes = max(bytemap.keys(), key=lambda b: len(b))
    if len(max_code_bytes) > 2:
        raise NotImplementedError("Only up to 2-byte encodings supported in this generator.")

    table_size = 256 * 256  # max for 2-byte encodings
    utf8_byte_map = [0x00] * (table_size * max_utf8_len)

    for code_bytes, utf8_bytes in bytemap.items():
        if len(code_bytes) == 1:
            offset = code_bytes[0]
        elif len(code_bytes) == 2:
            offset = code_bytes[0] * 256 + code_bytes[1]
        else:
            continue  # unsupported length

        base = offset * max_utf8_len
        for i in range(len(utf8_bytes)):
            utf8_byte_map[base + i] = utf8_bytes[i]

    # Format the array
    lines = []
    for i in range(0, len(utf8_byte_map), 16):
        chunk = utf8_byte_map[i:i+16]
        line = ', '.join(f'0x{b:02X}' for b in chunk)
        lines.append(f"    {line}")

    cpp_array = "\n".join(lines)
    cpp_code = f"""// Generated from: {filepath}
// Max UTF-8 bytes per encoded character: {max_utf8_len}
static constexpr uint8_t {encoding_name}_to_utf8[{len(utf8_byte_map)}] = {{
{cpp_array}
}};
"""
    return cpp_code

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python ucm_to_cpp_map.py <path_to_ucm_file>")
        sys.exit(1)

    filepath = sys.argv[1]
    encoding_name, bytemap, max_utf8_len = parse_ucm(filepath)
    cpp_output = generate_cpp_map(encoding_name, bytemap, max_utf8_len, filepath)

    out_filename = f"{encoding_name}_map.cpp"
    with open(out_filename, "w", encoding="utf-8") as out_file:
        out_file.write(cpp_output)

    print(f"C++ byte map written to {out_filename}")