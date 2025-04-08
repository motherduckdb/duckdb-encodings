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

namespace duckdb {

inline void EncodingsScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "Encodings "+name.GetString()+" 🐥");
        });
}

inline void EncodingsOpenSSLVersionScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "Encodings " + name.GetString() +
                                                     ", my linked OpenSSL version is " +
                                                     OPENSSL_VERSION_TEXT );
        });
}

static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    auto encodings_scalar_function = ScalarFunction("encodings", {LogicalType::VARCHAR}, LogicalType::VARCHAR, EncodingsScalarFun);
    ExtensionUtil::RegisterFunction(instance, encodings_scalar_function);

    // Register another scalar function
    auto encodings_openssl_version_scalar_function = ScalarFunction("encodings_openssl_version", {LogicalType::VARCHAR},
                                                LogicalType::VARCHAR, EncodingsOpenSSLVersionScalarFun);
    ExtensionUtil::RegisterFunction(instance, encodings_openssl_version_scalar_function);
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
