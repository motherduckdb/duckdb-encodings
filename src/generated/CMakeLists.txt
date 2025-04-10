add_library_unity(
  duckdb_encodings_extension
  OBJECT
  encoding_ascii_map.cpp
  encoding_big5_map.cpp
  )
set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:duckdb_encodings_extension>
  PARENT_SCOPE)
