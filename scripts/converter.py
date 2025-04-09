import sys
import os
import re

python_icu_encoding_map = [('ascii', '646', None, 'solaris-646-2.7.ucm'), ('big5', 'big5', None, 'aix-big5-4.3.6.ucm'), ('cp037', 'IBM037', None, 'glibc-IBM037-2.1.2.ucm'), ('cp273', 'IBM273', None, 'glibc-IBM273-2.1.2.ucm'), ('cp424', 'IBM424', None, 'glibc-IBM424-2.1.2.ucm'), ('cp437', 'IBM437', None, 'glibc-IBM437-2.1.2.ucm'), ('cp500', 'IBM500', None, 'glibc-IBM500-2.1.2.ucm'), ('cp850', 'IBM850', None, 'glibc-IBM850-2.1.2.ucm'), ('cp852', 'IBM852', None, 'glibc-IBM852-2.1.2.ucm'), ('cp855', 'IBM855', None, 'glibc-IBM855-2.1.2.ucm'), ('cp857', 'IBM857', None, 'glibc-IBM857-2.1.2.ucm'), ('cp860', 'IBM860', None, 'glibc-IBM860-2.1.2.ucm'), ('cp861', 'IBM861', None, 'glibc-IBM861-2.1.2.ucm'), ('cp862', 'IBM862', None, 'glibc-IBM862-2.1.2.ucm'), ('cp863', 'IBM863', None, 'glibc-IBM863-2.1.2.ucm'), ('cp864', 'IBM864', None, 'glibc-IBM864-2.1.2.ucm'), ('cp865', 'IBM865', None, 'glibc-IBM865-2.1.2.ucm'), ('cp866', 'IBM866', None, 'glibc-IBM866-2.1.2.ucm'), ('cp869', 'IBM869', None, 'glibc-IBM869-2.1.2.ucm'), ('cp949', 'UHC', None, 'glibc-UHC-2.1.2.ucm'), ('cp1026', 'IBM1026', None, 'glibc-IBM1026-2.1.2.ucm'), ('euc_jp', 'eucJP', None, 'solaris-eucJP-2.7.ucm'), ('gb18030', 'gb18030', None, 'gb-18030-2005.ucm'), ('mac_roman', 'MACINTOSH', None, 'glibc-MACINTOSH-2.1.2.ucm'), ('shift_jis', 'SJIS', None, 'glibc-SJIS-2.1.2.ucm')]

def unicode_to_utf8_bytes(codepoint: int) -> list[int]:
    """Convert a Unicode codepoint to a list of UTF-8 bytes."""
    return list(chr(codepoint).encode("utf-8"))

def parse_ucm(file_path):
    bytemap = {}
    max_utf8_len = 0

    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    in_charmap = False
    for line in lines:
        line = line.strip()
        if not line or line.startswith('#'):
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


    return bytemap, max_utf8_len

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

def generate_decoded_chars(python_icu_encoding_map, output_dir="data"):
    os.makedirs(output_dir, exist_ok=True)

    for encoding in python_icu_encoding_map:
        print(f"Generating for {encoding} (max {max_len} bytes)")
        path = os.path.join(output_dir, f"{encoding}.txt")
        seen = set()

        with open(path, "w", encoding="utf-8") as f_out:
            for byte_len in range(1, max_len + 1):
                for byte_seq in product(range(256), repeat=byte_len):
                    b = bytes(byte_seq)
                    try:
                        decoded = b.decode(encoding)
                        if decoded and decoded not in seen:
                            f_out.write(decoded + "\n")
                            seen.add(decoded)
                    except Exception:
                        continue



generated_path = os.path.join('src','generated')
os.makedirs(generated_path, exist_ok=True)

# Let's also generate the make file
with open(os.path.join(generated_path, 'CMakeLists.txt'), "w", encoding="utf-8") as cmake_file:
    cmake_file.write("add_library_unity(\n")
    cmake_file.write("  duckdb_encodings_extension\n")
    cmake_file.write("  OBJECT\n")
    
        

    for encoding in python_icu_encoding_map:
        encoding_map_file_path = os.path.join(generated_path,f'encoding_{encoding[0]}_map.cpp')
        with open(encoding_map_file_path, "w", encoding="utf-8") as out_file:
            # These are the matched encoding
            bytemap, max_utf8_len = parse_ucm(os.path.join('third_party','icu_encodings',encoding[3]))
            max_code_bytes = max(bytemap.keys(), key=lambda b: len(b))
            if len(max_code_bytes) > 2:
                continue
            cmake_file.write(f'  {encoding[0]}.cpp\n')
            cpp_output = generate_cpp_map(encoding[0], bytemap, max_utf8_len, encoding[3])
            out_file.write(cpp_output)
    cmake_file.write('  )\n') 
    cmake_file.write('set(ALL_OBJECT_FILES\n')
    cmake_file.write('  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:duckdb_encodings_extension>\n')
    cmake_file.write('  PARENT_SCOPE)\n')


# def generate_cpp_maps():
#     # for 
#     encoding_name, bytemap, max_utf8_len = parse_ucm(filepath)
#     cpp_output = generate_cpp_map(encoding_name, bytemap, max_utf8_len, filepath)

#     out_filename = f"{encoding_name}_map.cpp"
#     with open(out_filename, "w", encoding="utf-8") as out_file:
#         out_file.write(cpp_output)

#     print(f"C++ byte map written to {out_filename}")