PROJ_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Configuration of extension
EXT_NAME=encodings
EXT_CONFIG=${PROJ_DIR}extension_config.cmake

# Include the Makefile from extension-ci-tools
include extension-ci-tools/makefiles/duckdb_extension.Makefile

# Override WASM targets: THIS IS AN HACK, needs fixing in DuckDB to allow passing optimization level and other options, but still allows this to be build as out of tree extension
wasm_pre_build_step:
	cd duckdb && git apply --3way ../cmakelists.patch && cd ..
