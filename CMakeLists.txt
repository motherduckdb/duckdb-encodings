cmake_minimum_required(VERSION 3.5)

# Set extension name here
set(TARGET_NAME encodings)

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})
include_directories(src/include)

set(EXTENSION_SOURCES src/encodings_extension.cpp
                      src/generated_encoded_functions.cpp)

add_subdirectory(src/generated)

build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES} ${ALL_OBJECT_FILES})
build_loadable_extension(${TARGET_NAME} "${ALL_OBJECT_FILES}"
                         ${EXTENSION_SOURCES})

if(MSVC)
  add_compile_options(/bigobj)
endif()

set_property(TARGET ${EXTENSION_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${EXTENSION_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${LOADABLE_EXTENSION_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${LOADABLE_EXTENSION_NAME} PROPERTY CXX_STANDARD_REQUIRED
                                                        ON)

install(
  TARGETS ${EXTENSION_NAME}
  EXPORT "${DUCKDB_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
