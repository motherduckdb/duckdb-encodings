#define DUCKDB_EXTENSION_MAIN

#include "encodings_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

// OpenSSL linked through vcpkg
#include <openssl/opensslv.h>

#include "generated_encoded_function.hpp"
#include "generated/registration.hpp"

namespace duckdb {

static void LoadInternal(DatabaseInstance &instance) {
	// Register a scalar function
	duckdb_encodings::RegistrationEncodedFunctions::RegisterFunctions(instance.config);
}

void EncodingsExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string EncodingsExtension::Name() {
	return "encodings";
}

std::string EncodingsExtension::Version() const {
#ifdef EXT_VERSION_ENCODINGS
	return EXT_VERSION_ENCODINGS;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void encodings_init(duckdb::DatabaseInstance &db) {
	duckdb::DuckDB db_wrapper(db);
	db_wrapper.LoadExtension<duckdb::EncodingsExtension>();
}

DUCKDB_EXTENSION_API const char *encodings_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
