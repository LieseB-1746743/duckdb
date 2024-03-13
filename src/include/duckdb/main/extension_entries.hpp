//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/main/extension_entries.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/unordered_map.hpp"
#include "duckdb/common/enums/catalog_type.hpp"

// NOTE: this file is generated by scripts/generate_extensions_function.py. Check out the check-load-install-extensions
//       job in .github/workflows/LinuxRelease.yml on how to use it

namespace duckdb {

struct ExtensionEntry {
	char name[48];
	char extension[48];
};

struct ExtensionFunctionEntry {
	char name[48];
	char extension[48];
	CatalogType type;
};

static constexpr ExtensionFunctionEntry EXTENSION_FUNCTIONS[] = {
    {"->>", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"add_parquet_key", "parquet", CatalogType::PRAGMA_FUNCTION_ENTRY},
    {"array_to_json", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"create_fts_index", "fts", CatalogType::PRAGMA_FUNCTION_ENTRY},
    {"current_localtime", "icu", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"current_localtimestamp", "icu", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"dbgen", "tpch", CatalogType::TABLE_FUNCTION_ENTRY},
    {"drop_fts_index", "fts", CatalogType::PRAGMA_FUNCTION_ENTRY},
    {"dsdgen", "tpcds", CatalogType::TABLE_FUNCTION_ENTRY},
    {"excel_text", "excel", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"family", "inet", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"from_json", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"from_json_strict", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"from_substrait", "substrait", CatalogType::TABLE_FUNCTION_ENTRY},
    {"from_substrait_json", "substrait", CatalogType::TABLE_FUNCTION_ENTRY},
    {"fuzz_all_functions", "sqlsmith", CatalogType::TABLE_FUNCTION_ENTRY},
    {"fuzzyduck", "sqlsmith", CatalogType::TABLE_FUNCTION_ENTRY},
    {"get_substrait", "substrait", CatalogType::TABLE_FUNCTION_ENTRY},
    {"get_substrait_json", "substrait", CatalogType::TABLE_FUNCTION_ENTRY},
    {"host", "inet", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"iceberg_metadata", "iceberg", CatalogType::TABLE_FUNCTION_ENTRY},
    {"iceberg_scan", "iceberg", CatalogType::TABLE_FUNCTION_ENTRY},
    {"iceberg_snapshots", "iceberg", CatalogType::TABLE_FUNCTION_ENTRY},
    {"icu_calendar_names", "icu", CatalogType::TABLE_FUNCTION_ENTRY},
    {"icu_sort_key", "icu", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json", "json", CatalogType::MACRO_ENTRY},
    {"json_array", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_array_length", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_contains", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_deserialize_sql", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_execute_serialized_sql", "json", CatalogType::PRAGMA_FUNCTION_ENTRY},
    {"json_execute_serialized_sql", "json", CatalogType::TABLE_FUNCTION_ENTRY},
    {"json_extract", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_extract_path", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_extract_path_text", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_extract_string", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_group_array", "json", CatalogType::MACRO_ENTRY},
    {"json_group_object", "json", CatalogType::MACRO_ENTRY},
    {"json_group_structure", "json", CatalogType::MACRO_ENTRY},
    {"json_keys", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_merge_patch", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_object", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_quote", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_serialize_plan", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_serialize_sql", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_structure", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_transform", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_transform_strict", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_type", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"json_valid", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"load_aws_credentials", "aws", CatalogType::TABLE_FUNCTION_ENTRY},
    {"make_timestamptz", "icu", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"parquet_file_metadata", "parquet", CatalogType::TABLE_FUNCTION_ENTRY},
    {"parquet_kv_metadata", "parquet", CatalogType::TABLE_FUNCTION_ENTRY},
    {"parquet_metadata", "parquet", CatalogType::TABLE_FUNCTION_ENTRY},
    {"parquet_scan", "parquet", CatalogType::TABLE_FUNCTION_ENTRY},
    {"parquet_schema", "parquet", CatalogType::TABLE_FUNCTION_ENTRY},
    {"pg_clear_cache", "postgres_scanner", CatalogType::TABLE_FUNCTION_ENTRY},
    {"pg_timezone_names", "icu", CatalogType::TABLE_FUNCTION_ENTRY},
    {"postgres_attach", "postgres_scanner", CatalogType::TABLE_FUNCTION_ENTRY},
    {"postgres_query", "postgres_scanner", CatalogType::TABLE_FUNCTION_ENTRY},
    {"postgres_scan", "postgres_scanner", CatalogType::TABLE_FUNCTION_ENTRY},
    {"postgres_scan_pushdown", "postgres_scanner", CatalogType::TABLE_FUNCTION_ENTRY},
    {"read_json", "json", CatalogType::TABLE_FUNCTION_ENTRY},
    {"read_json_auto", "json", CatalogType::TABLE_FUNCTION_ENTRY},
    {"read_json_objects", "json", CatalogType::TABLE_FUNCTION_ENTRY},
    {"read_json_objects_auto", "json", CatalogType::TABLE_FUNCTION_ENTRY},
    {"read_ndjson", "json", CatalogType::TABLE_FUNCTION_ENTRY},
    {"read_ndjson_auto", "json", CatalogType::TABLE_FUNCTION_ENTRY},
    {"read_ndjson_objects", "json", CatalogType::TABLE_FUNCTION_ENTRY},
    {"read_parquet", "parquet", CatalogType::TABLE_FUNCTION_ENTRY},
    {"reduce_sql_statement", "sqlsmith", CatalogType::TABLE_FUNCTION_ENTRY},
    {"row_to_json", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"scan_arrow_ipc", "arrow", CatalogType::TABLE_FUNCTION_ENTRY},
    {"shapefile_meta", "spatial", CatalogType::TABLE_FUNCTION_ENTRY},
    {"sql_auto_complete", "autocomplete", CatalogType::TABLE_FUNCTION_ENTRY},
    {"sqlite_attach", "sqlite_scanner", CatalogType::TABLE_FUNCTION_ENTRY},
    {"sqlite_scan", "sqlite_scanner", CatalogType::TABLE_FUNCTION_ENTRY},
    {"sqlsmith", "sqlsmith", CatalogType::TABLE_FUNCTION_ENTRY},
    {"st_area", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_area_spheroid", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_asgeojson", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_ashexwkb", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_astext", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_aswkb", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_boundary", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_buffer", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_centroid", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_collect", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_collectionextract", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_contains", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_containsproperly", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_convexhull", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_coveredby", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_covers", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_crosses", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_difference", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_dimension", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_disjoint", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_distance", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_distance_spheroid", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_drivers", "spatial", CatalogType::TABLE_FUNCTION_ENTRY},
    {"st_dump", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_dwithin", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_dwithin_spheroid", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_endpoint", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_envelope", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_envelope_agg", "spatial", CatalogType::AGGREGATE_FUNCTION_ENTRY},
    {"st_equals", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_extent", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_exteriorring", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_flipcoordinates", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_geometrytype", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_geomfromgeojson", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_geomfromhexewkb", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_geomfromhexwkb", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_geomfromtext", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_geomfromwkb", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_intersection", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_intersection_agg", "spatial", CatalogType::AGGREGATE_FUNCTION_ENTRY},
    {"st_intersects", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_intersects_extent", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_isclosed", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_isempty", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_isring", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_issimple", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_isvalid", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_length", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_length_spheroid", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_linemerge", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_linestring2dfromwkb", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_list_proj_crs", "spatial", CatalogType::TABLE_FUNCTION_ENTRY},
    {"st_makeenvelope", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_makeline", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_makepolygon", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_makevalid", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_ngeometries", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_ninteriorrings", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_normalize", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_npoints", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_numgeometries", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_numinteriorrings", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_numpoints", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_overlaps", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_perimeter", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_perimeter_spheroid", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_point", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_point2d", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_point2dfromwkb", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_point3d", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_point4d", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_pointn", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_pointonsurface", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_polygon2dfromwkb", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_quadkey", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_read", "spatial", CatalogType::TABLE_FUNCTION_ENTRY},
    {"st_read_meta", "spatial", CatalogType::TABLE_FUNCTION_ENTRY},
    {"st_readosm", "spatial", CatalogType::TABLE_FUNCTION_ENTRY},
    {"st_readshp", "spatial", CatalogType::TABLE_FUNCTION_ENTRY},
    {"st_reduceprecision", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_removerepeatedpoints", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_reverse", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_simplify", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_simplifypreservetopology", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_startpoint", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_touches", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_transform", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_union", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_union_agg", "spatial", CatalogType::AGGREGATE_FUNCTION_ENTRY},
    {"st_within", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_x", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_xmax", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_xmin", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_y", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_ymax", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_ymin", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_z", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_zmax", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_zmin", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_m", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_mmax", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_mmin", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_force2d", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_force3dz", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_force3dm", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"st_force4d", "spatial", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"stem", "fts", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"text", "excel", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"to_arrow_ipc", "arrow", CatalogType::TABLE_FUNCTION_ENTRY},
    {"to_json", "json", CatalogType::SCALAR_FUNCTION_ENTRY},
    {"tpcds", "tpcds", CatalogType::PRAGMA_FUNCTION_ENTRY},
    {"tpcds_answers", "tpcds", CatalogType::TABLE_FUNCTION_ENTRY},
    {"tpcds_queries", "tpcds", CatalogType::TABLE_FUNCTION_ENTRY},
    {"tpch", "tpch", CatalogType::PRAGMA_FUNCTION_ENTRY},
    {"tpch_answers", "tpch", CatalogType::TABLE_FUNCTION_ENTRY},
    {"tpch_queries", "tpch", CatalogType::TABLE_FUNCTION_ENTRY},
}; // END_OF_EXTENSION_FUNCTIONS

static constexpr ExtensionEntry EXTENSION_SETTINGS[] = {
    {"azure_account_name", "azure"},
    {"azure_credential_chain", "azure"},
    {"azure_endpoint", "azure"},
    {"azure_http_proxy", "azure"},
    {"azure_http_stats", "azure"},
    {"azure_proxy_password", "azure"},
    {"azure_proxy_user_name", "azure"},
    {"azure_read_buffer_size", "azure"},
    {"azure_read_transfer_chunk_size", "azure"},
    {"azure_read_transfer_concurrency", "azure"},
    {"azure_storage_connection_string", "azure"},
    {"binary_as_string", "parquet"},
    {"ca_cert_file", "httpfs"},
    {"calendar", "icu"},
    {"enable_server_cert_verification", "httpfs"},
    {"force_download", "httpfs"},
    {"http_keep_alive", "httpfs"},
    {"http_retries", "httpfs"},
    {"http_retry_backoff", "httpfs"},
    {"http_retry_wait_ms", "httpfs"},
    {"http_timeout", "httpfs"},
    {"pg_array_as_varchar", "postgres_scanner"},
    {"pg_connection_cache", "postgres_scanner"},
    {"pg_connection_limit", "postgres_scanner"},
    {"pg_debug_show_queries", "postgres_scanner"},
    {"pg_experimental_filter_pushdown", "postgres_scanner"},
    {"pg_pages_per_task", "postgres_scanner"},
    {"pg_use_binary_copy", "postgres_scanner"},
    {"s3_access_key_id", "httpfs"},
    {"s3_endpoint", "httpfs"},
    {"s3_region", "httpfs"},
    {"s3_secret_access_key", "httpfs"},
    {"s3_session_token", "httpfs"},
    {"s3_uploader_max_filesize", "httpfs"},
    {"s3_uploader_max_parts_per_file", "httpfs"},
    {"s3_uploader_thread_limit", "httpfs"},
    {"s3_url_compatibility_mode", "httpfs"},
    {"s3_url_style", "httpfs"},
    {"s3_use_ssl", "httpfs"},
    {"sqlite_all_varchar", "sqlite_scanner"},
    {"timezone", "icu"},
}; // END_OF_EXTENSION_SETTINGS

// Note: these are currently hardcoded in scripts/generate_extensions_function.py
// TODO: automate by passing though to script via duckdb
static constexpr ExtensionEntry EXTENSION_COPY_FUNCTIONS[] = {{"parquet", "parquet"},
                                                              {"json", "json"}}; // END_OF_EXTENSION_COPY_FUNCTIONS

// Note: these are currently hardcoded in scripts/generate_extensions_function.py
// TODO: automate by passing though to script via duckdb
static constexpr ExtensionEntry EXTENSION_TYPES[] = {
    {"json", "json"}, {"inet", "inet"}, {"geometry", "spatial"}}; // END_OF_EXTENSION_TYPES

// Note: these are currently hardcoded in scripts/generate_extensions_function.py
// TODO: automate by passing though to script via duckdb
static constexpr ExtensionEntry EXTENSION_COLLATIONS[] = {
    {"af", "icu"},    {"am", "icu"},    {"ar", "icu"},     {"ar_sa", "icu"}, {"as", "icu"},    {"az", "icu"},
    {"be", "icu"},    {"bg", "icu"},    {"bn", "icu"},     {"bo", "icu"},    {"br", "icu"},    {"bs", "icu"},
    {"ca", "icu"},    {"ceb", "icu"},   {"chr", "icu"},    {"cs", "icu"},    {"cy", "icu"},    {"da", "icu"},
    {"de", "icu"},    {"de_at", "icu"}, {"dsb", "icu"},    {"dz", "icu"},    {"ee", "icu"},    {"el", "icu"},
    {"en", "icu"},    {"en_us", "icu"}, {"eo", "icu"},     {"es", "icu"},    {"et", "icu"},    {"fa", "icu"},
    {"fa_af", "icu"}, {"ff", "icu"},    {"fi", "icu"},     {"fil", "icu"},   {"fo", "icu"},    {"fr", "icu"},
    {"fr_ca", "icu"}, {"fy", "icu"},    {"ga", "icu"},     {"gl", "icu"},    {"gu", "icu"},    {"ha", "icu"},
    {"haw", "icu"},   {"he", "icu"},    {"he_il", "icu"},  {"hi", "icu"},    {"hr", "icu"},    {"hsb", "icu"},
    {"hu", "icu"},    {"hy", "icu"},    {"id", "icu"},     {"id_id", "icu"}, {"ig", "icu"},    {"is", "icu"},
    {"it", "icu"},    {"ja", "icu"},    {"ka", "icu"},     {"kk", "icu"},    {"kl", "icu"},    {"km", "icu"},
    {"kn", "icu"},    {"ko", "icu"},    {"kok", "icu"},    {"ku", "icu"},    {"ky", "icu"},    {"lb", "icu"},
    {"lkt", "icu"},   {"ln", "icu"},    {"lo", "icu"},     {"lt", "icu"},    {"lv", "icu"},    {"mk", "icu"},
    {"ml", "icu"},    {"mn", "icu"},    {"mr", "icu"},     {"ms", "icu"},    {"mt", "icu"},    {"my", "icu"},
    {"nb", "icu"},    {"nb_no", "icu"}, {"ne", "icu"},     {"nl", "icu"},    {"nn", "icu"},    {"om", "icu"},
    {"or", "icu"},    {"pa", "icu"},    {"pa_in", "icu"},  {"pl", "icu"},    {"ps", "icu"},    {"pt", "icu"},
    {"ro", "icu"},    {"ru", "icu"},    {"sa", "icu"},     {"se", "icu"},    {"si", "icu"},    {"sk", "icu"},
    {"sl", "icu"},    {"smn", "icu"},   {"sq", "icu"},     {"sr", "icu"},    {"sr_ba", "icu"}, {"sr_me", "icu"},
    {"sr_rs", "icu"}, {"sv", "icu"},    {"sw", "icu"},     {"ta", "icu"},    {"te", "icu"},    {"th", "icu"},
    {"tk", "icu"},    {"to", "icu"},    {"tr", "icu"},     {"ug", "icu"},    {"uk", "icu"},    {"ur", "icu"},
    {"uz", "icu"},    {"vi", "icu"},    {"wae", "icu"},    {"wo", "icu"},    {"xh", "icu"},    {"yi", "icu"},
    {"yo", "icu"},    {"yue", "icu"},   {"yue_cn", "icu"}, {"zh", "icu"},    {"zh_cn", "icu"}, {"zh_hk", "icu"},
    {"zh_mo", "icu"}, {"zh_sg", "icu"}, {"zh_tw", "icu"},  {"zu", "icu"}}; // END_OF_EXTENSION_COLLATIONS

// Note: these are currently hardcoded in scripts/generate_extensions_function.py
// TODO: automate by passing though to script via duckdb
static constexpr ExtensionEntry EXTENSION_FILE_PREFIXES[] = {
    {"http://", "httpfs"}, {"https://", "httpfs"}, {"s3://", "httpfs"}, {"s3a://", "httpfs"}, {"s3n://", "httpfs"},
    {"gcs://", "httpfs"},  {"gs://", "httpfs"},    {"r2://", "httpfs"}
    //    {"azure://", "azure"}
}; // END_OF_EXTENSION_FILE_PREFIXES

// Note: these are currently hardcoded in scripts/generate_extensions_function.py
// TODO: automate by passing though to script via duckdb
static constexpr ExtensionEntry EXTENSION_FILE_POSTFIXES[] = {
    {".parquet", "parquet"}, {".json", "json"},    {".jsonl", "json"}, {".ndjson", "json"},
    {".shp", "spatial"},     {".gpkg", "spatial"}, {".fgb", "spatial"}}; // END_OF_EXTENSION_FILE_POSTFIXES

// Note: these are currently hardcoded in scripts/generate_extensions_function.py
// TODO: automate by passing though to script via duckdb
static constexpr ExtensionEntry EXTENSION_FILE_CONTAINS[] = {{".parquet?", "parquet"},
                                                             {".json?", "json"},
                                                             {".ndjson?", ".jsonl?"},
                                                             {".jsonl?", ".ndjson?"}}; // EXTENSION_FILE_CONTAINS

// Note: these are currently hardcoded in scripts/generate_extensions_function.py
// TODO: automate by passing though to script via duckdb
static constexpr ExtensionEntry EXTENSION_SECRET_TYPES[] = {
    {"s3", "httpfs"}, {"r2", "httpfs"}, {"gcs", "httpfs"}, {"azure", "azure"}}; // EXTENSION_SECRET_TYPES

// Note: these are currently hardcoded in scripts/generate_extensions_function.py
// TODO: automate by passing though to script via duckdb
static constexpr ExtensionEntry EXTENSION_SECRET_PROVIDERS[] = {
    {"s3/config", "httpfs"},        {"gcs/config", "httpfs"},           {"r2/config", "httpfs"},
    {"s3/credential_chain", "aws"}, {"gcs/credential_chain", "aws"},    {"r2/credential_chain", "aws"},
    {"azure/config", "azure"},      {"azure/credential_chain", "azure"}}; // EXTENSION_SECRET_PROVIDERS

static constexpr const char *AUTOLOADABLE_EXTENSIONS[] = {
    "aws", "azure", "autocomplete", "excel",          "fts",      "httpfs",           "inet",
    "icu", "json",  "parquet",      "sqlite_scanner", "sqlsmith", "postgres_scanner", "tpcds",
    "tpch"}; // END_OF_AUTOLOADABLE_EXTENSIONS

} // namespace duckdb
