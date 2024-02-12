//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/main/extension_entries.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/unordered_map.hpp"

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
	char type[48];
};

static constexpr ExtensionFunctionEntry EXTENSION_FUNCTIONS[] = {
    {"->>", "json", "scalar"},
    {"array_to_json", "json", "scalar"},
    {"create_fts_index", "fts", "pragma"},
    {"current_localtime", "icu", "scalar"},
    {"current_localtimestamp", "icu", "scalar"},
    {"dbgen", "tpch", "table"},
    {"drop_fts_index", "fts", "pragma"},
    {"dsdgen", "tpcds", "table"},
    {"excel_text", "excel", "scalar"},
    {"from_json", "json", "scalar"},
    {"from_json_strict", "json", "scalar"},
    {"from_substrait", "substrait", "table"},
    {"from_substrait_json", "substrait", "table"},
    {"fuzz_all_functions", "sqlsmith", "table"},
    {"fuzzyduck", "sqlsmith", "table"},
    {"get_substrait", "substrait", "table"},
    {"get_substrait_json", "substrait", "table"},
    {"host", "inet", "scalar"},
    {"iceberg_metadata", "iceberg", "table"},
    {"iceberg_scan", "iceberg", "table"},
    {"iceberg_snapshots", "iceberg", "table"},
    {"icu_calendar_names", "icu", "table"},
    {"icu_sort_key", "icu", "scalar"},
    {"json", "json", "macro"},
    {"json_array", "json", "scalar"},
    {"json_array_length", "json", "scalar"},
    {"json_contains", "json", "scalar"},
    {"json_deserialize_sql", "json", "scalar"},
    {"json_execute_serialized_sql", "json", "pragma"},
    {"json_extract", "json", "scalar"},
    {"json_extract_path", "json", "scalar"},
    {"json_extract_path_text", "json", "scalar"},
    {"json_extract_string", "json", "scalar"},
    {"json_group_array", "json", "macro"},
    {"json_group_object", "json", "macro"},
    {"json_group_structure", "json", "macro"},
    {"json_keys", "json", "scalar"},
    {"json_merge_patch", "json", "scalar"},
    {"json_object", "json", "scalar"},
    {"json_quote", "json", "scalar"},
    {"json_serialize_plan", "json", "scalar"},
    {"json_serialize_sql", "json", "scalar"},
    {"json_structure", "json", "scalar"},
    {"json_transform", "json", "scalar"},
    {"json_transform_strict", "json", "scalar"},
    {"json_type", "json", "scalar"},
    {"json_valid", "json", "scalar"},
    {"load_aws_credentials", "aws", "table"},
    {"make_timestamptz", "icu", "scalar"},
    {"pg_clear_cache", "postgres_scanner", "table"},
    {"pg_timezone_names", "icu", "table"},
    {"postgres_attach", "postgres_scanner", "table"},
    {"postgres_query", "postgres_scanner", "table"},
    {"postgres_scan", "postgres_scanner", "table"},
    {"postgres_scan_pushdown", "postgres_scanner", "table"},
    {"read_json", "json", "table"},
    {"read_json_auto", "json", "table"},
    {"read_json_objects", "json", "table"},
    {"read_json_objects_auto", "json", "table"},
    {"read_ndjson", "json", "table"},
    {"read_ndjson_auto", "json", "table"},
    {"read_ndjson_objects", "json", "table"},
    {"reduce_sql_statement", "sqlsmith", "table"},
    {"row_to_json", "json", "scalar"},
    {"scan_arrow_ipc", "arrow", "table"},
    {"shapefile_meta", "spatial", "table"},
    {"sql_auto_complete", "autocomplete", "table"},
    {"sqlite_attach", "sqlite_scanner", "table"},
    {"sqlite_scan", "sqlite_scanner", "table"},
    {"sqlsmith", "sqlsmith", "table"},
    {"st_area", "spatial", "scalar"},
    {"st_area_spheroid", "spatial", "scalar"},
    {"st_asgeojson", "spatial", "scalar"},
    {"st_ashexwkb", "spatial", "scalar"},
    {"st_astext", "spatial", "scalar"},
    {"st_aswkb", "spatial", "scalar"},
    {"st_boundary", "spatial", "scalar"},
    {"st_buffer", "spatial", "scalar"},
    {"st_centroid", "spatial", "scalar"},
    {"st_collect", "spatial", "scalar"},
    {"st_collectionextract", "spatial", "scalar"},
    {"st_contains", "spatial", "scalar"},
    {"st_containsproperly", "spatial", "scalar"},
    {"st_convexhull", "spatial", "scalar"},
    {"st_coveredby", "spatial", "scalar"},
    {"st_covers", "spatial", "scalar"},
    {"st_crosses", "spatial", "scalar"},
    {"st_difference", "spatial", "scalar"},
    {"st_dimension", "spatial", "scalar"},
    {"st_disjoint", "spatial", "scalar"},
    {"st_distance", "spatial", "scalar"},
    {"st_distance_spheroid", "spatial", "scalar"},
    {"st_drivers", "spatial", "table"},
    {"st_dump", "spatial", "scalar"},
    {"st_dwithin", "spatial", "scalar"},
    {"st_dwithin_spheroid", "spatial", "scalar"},
    {"st_endpoint", "spatial", "scalar"},
    {"st_envelope", "spatial", "scalar"},
    {"st_envelope_agg", "spatial", "aggregate"},
    {"st_equals", "spatial", "scalar"},
    {"st_extent", "spatial", "scalar"},
    {"st_exteriorring", "spatial", "scalar"},
    {"st_flipcoordinates", "spatial", "scalar"},
    {"st_geometrytype", "spatial", "scalar"},
    {"st_geomfromgeojson", "spatial", "scalar"},
    {"st_geomfromhexewkb", "spatial", "scalar"},
    {"st_geomfromhexwkb", "spatial", "scalar"},
    {"st_geomfromtext", "spatial", "scalar"},
    {"st_geomfromwkb", "spatial", "scalar"},
    {"st_intersection", "spatial", "scalar"},
    {"st_intersection_agg", "spatial", "aggregate"},
    {"st_intersects", "spatial", "scalar"},
    {"st_intersects_extent", "spatial", "scalar"},
    {"st_isclosed", "spatial", "scalar"},
    {"st_isempty", "spatial", "scalar"},
    {"st_isring", "spatial", "scalar"},
    {"st_issimple", "spatial", "scalar"},
    {"st_isvalid", "spatial", "scalar"},
    {"st_length", "spatial", "scalar"},
    {"st_length_spheroid", "spatial", "scalar"},
    {"st_linemerge", "spatial", "scalar"},
    {"st_linestring2dfromwkb", "spatial", "scalar"},
    {"st_list_proj_crs", "spatial", "table"},
    {"st_makeenvelope", "spatial", "scalar"},
    {"st_makeline", "spatial", "scalar"},
    {"st_makepolygon", "spatial", "scalar"},
    {"st_makevalid", "spatial", "scalar"},
    {"st_ngeometries", "spatial", "scalar"},
    {"st_ninteriorrings", "spatial", "scalar"},
    {"st_normalize", "spatial", "scalar"},
    {"st_npoints", "spatial", "scalar"},
    {"st_numgeometries", "spatial", "scalar"},
    {"st_numinteriorrings", "spatial", "scalar"},
    {"st_numpoints", "spatial", "scalar"},
    {"st_overlaps", "spatial", "scalar"},
    {"st_perimeter", "spatial", "scalar"},
    {"st_perimeter_spheroid", "spatial", "scalar"},
    {"st_point", "spatial", "scalar"},
    {"st_point2d", "spatial", "scalar"},
    {"st_point2dfromwkb", "spatial", "scalar"},
    {"st_point3d", "spatial", "scalar"},
    {"st_point4d", "spatial", "scalar"},
    {"st_pointn", "spatial", "scalar"},
    {"st_pointonsurface", "spatial", "scalar"},
    {"st_polygon2dfromwkb", "spatial", "scalar"},
    {"st_quadkey", "spatial", "scalar"},
    {"st_read", "spatial", "table"},
    {"st_read_meta", "spatial", "table"},
    {"st_readosm", "spatial", "table"},
    {"st_readshp", "spatial", "table"},
    {"st_reduceprecision", "spatial", "scalar"},
    {"st_removerepeatedpoints", "spatial", "scalar"},
    {"st_reverse", "spatial", "scalar"},
    {"st_simplify", "spatial", "scalar"},
    {"st_simplifypreservetopology", "spatial", "scalar"},
    {"st_startpoint", "spatial", "scalar"},
    {"st_touches", "spatial", "scalar"},
    {"st_transform", "spatial", "scalar"},
    {"st_union", "spatial", "scalar"},
    {"st_union_agg", "spatial", "aggregate"},
    {"st_within", "spatial", "scalar"},
    {"st_x", "spatial", "scalar"},
    {"st_xmax", "spatial", "scalar"},
    {"st_xmin", "spatial", "scalar"},
    {"st_y", "spatial", "scalar"},
    {"st_ymax", "spatial", "scalar"},
    {"st_ymin", "spatial", "scalar"},
    {"stem", "fts", "scalar"},
    {"text", "excel", "scalar"},
    {"to_arrow_ipc", "arrow", "table"},
    {"to_json", "json", "scalar"},
    {"tpcds", "tpcds", "pragma"},
    {"tpcds_answers", "tpcds", "table"},
    {"tpcds_queries", "tpcds", "table"},
    {"tpch", "tpch", "pragma"},
    {"tpch_answers", "tpch", "table"},
    {"tpch_queries", "tpch", "table"},
}; // END_OF_EXTENSION_FUNCTIONS

static constexpr ExtensionEntry EXTENSION_SETTINGS[] = {
    {"azure_storage_connection_string", "azure"},
    {"calendar", "icu"},
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
    //    "azure",
    "autocomplete", "excel", "fts", "httpfs",
    // "inet",
    // "icu",
    "json", "parquet", "sqlsmith", "tpcds", "tpch", "visualizer"}; // END_OF_AUTOLOADABLE_EXTENSIONS

} // namespace duckdb
