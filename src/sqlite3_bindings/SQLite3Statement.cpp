#include "SQLite3Statement.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

SQLite3Statement::SQLite3Statement() : _stmt(nullptr) {}

SQLite3Statement::SQLite3Statement(sqlite3_stmt* stmt) : _stmt(stmt) {}

SQLite3Statement::~SQLite3Statement() {
    if (_stmt) {
        sqlite3_finalize(_stmt);
    }
}

String SQLite3Statement::sql() {
    return _stmt ? String(sqlite3_sql(_stmt)) : String();
}

String SQLite3Statement::expanded_sql() {
    if (!_stmt) return String();
    char* sql = sqlite3_expanded_sql(_stmt);
    String s = String(sql);
    sqlite3_free(sql);
    return s;
}



bool SQLite3Statement::readonly() {
    return _stmt ? sqlite3_stmt_readonly(_stmt) : false;
}

bool SQLite3Statement::isexplain() {
    return _stmt ? sqlite3_stmt_isexplain(_stmt) : false;
}

int SQLite3Statement::explain(int eMode) {
    return _stmt ? sqlite3_stmt_explain(_stmt, eMode) : SQLITE_MISUSE;
}

bool SQLite3Statement::busy() {
    return _stmt ? sqlite3_stmt_busy(_stmt) : false;
}

int SQLite3Statement::bind_blob(int index, const PackedByteArray& value, bool transient) {
    if (!_stmt) return SQLITE_MISUSE;
    return sqlite3_bind_blob(_stmt, index, value.ptr(), value.size(), transient ? SQLITE_TRANSIENT : SQLITE_STATIC);
}

int SQLite3Statement::bind_blob64(int index, const PackedByteArray& value, bool transient) {
    if (!_stmt) return SQLITE_MISUSE;
    return sqlite3_bind_blob64(_stmt, index, value.ptr(), value.size(), transient ? SQLITE_TRANSIENT : SQLITE_STATIC);
}

int SQLite3Statement::bind_double(int index, double value) {
    return _stmt ? sqlite3_bind_double(_stmt, index, value) : SQLITE_MISUSE;
}

int SQLite3Statement::bind_int(int index, int value) {
    return _stmt ? sqlite3_bind_int(_stmt, index, value) : SQLITE_MISUSE;
}

int SQLite3Statement::bind_int64(int index, int64_t value) {
    return _stmt ? sqlite3_bind_int64(_stmt, index, value) : SQLITE_MISUSE;
}

int SQLite3Statement::bind_null(int index) {
    return _stmt ? sqlite3_bind_null(_stmt, index) : SQLITE_MISUSE;
}

int SQLite3Statement::bind_text(int index, const String& value, bool transient) {
    if (!_stmt) return SQLITE_MISUSE;
    return sqlite3_bind_text(_stmt, index, value.utf8().get_data(), -1, transient ? SQLITE_TRANSIENT : SQLITE_STATIC);
}

int SQLite3Statement::bind_text16(int index, const String& value, bool transient) {
    if (!_stmt) return SQLITE_MISUSE;
    return sqlite3_bind_text16(_stmt, index, value.utf16().ptrw(), -1, transient ? SQLITE_TRANSIENT : SQLITE_STATIC);
}

int SQLite3Statement::bind_text64(int index, const String& value, bool transient, int encoding) {
    if (!_stmt) return SQLITE_MISUSE;
    return sqlite3_bind_text64(_stmt, index, value.utf8().get_data(), value.length(), transient ? SQLITE_TRANSIENT : SQLITE_STATIC, encoding);
}

int SQLite3Statement::bind_value(int index, const Variant& value) {
    // Simplified: convert Variant to appropriate bind
    if (!_stmt) return SQLITE_MISUSE;
    switch (value.get_type()) {
        case Variant::Type::NIL:
            return bind_null(index);
        case Variant::Type::BOOL:
            return bind_int(index, value ? 1 : 0);
        case Variant::Type::INT:
            return bind_int64(index, value);
        case Variant::Type::FLOAT:
            return bind_double(index, value);
        case Variant::Type::STRING:
            return bind_text(index, value, true);
        case Variant::Type::PACKED_BYTE_ARRAY:
            return bind_blob(index, value, true);
        default:
            return SQLITE_MISUSE; // Not supported
    }
}



int SQLite3Statement::bind_zeroblob(int index, int n) {
    return _stmt ? sqlite3_bind_zeroblob(_stmt, index, n) : SQLITE_MISUSE;
}

int SQLite3Statement::bind_zeroblob64(int index, int64_t n) {
    return _stmt ? sqlite3_bind_zeroblob64(_stmt, index, n) : SQLITE_MISUSE;
}

int SQLite3Statement::bind_parameter_count() {
    return _stmt ? sqlite3_bind_parameter_count(_stmt) : 0;
}

String SQLite3Statement::bind_parameter_name(int index) {
    return _stmt ? String(sqlite3_bind_parameter_name(_stmt, index)) : String();
}

int SQLite3Statement::bind_parameter_index(const String& name) {
    return _stmt ? sqlite3_bind_parameter_index(_stmt, name.utf8().get_data()) : 0;
}

int SQLite3Statement::clear_bindings() {
    return _stmt ? sqlite3_clear_bindings(_stmt) : SQLITE_MISUSE;
}

int SQLite3Statement::step() {
    return _stmt ? sqlite3_step(_stmt) : SQLITE_MISUSE;
}

int SQLite3Statement::data_count() {
    return _stmt ? sqlite3_data_count(_stmt) : 0;
}

int SQLite3Statement::finalize() {
    if (!_stmt) return SQLITE_MISUSE;
    int rc = sqlite3_finalize(_stmt);
    _stmt = nullptr;
    return rc;
}

int SQLite3Statement::reset() {
    return _stmt ? sqlite3_reset(_stmt) : SQLITE_MISUSE;
}

PackedByteArray SQLite3Statement::column_blob(int iCol) {
    if (!_stmt) return PackedByteArray();
    const void* data = sqlite3_column_blob(_stmt, iCol);
    int size = sqlite3_column_bytes(_stmt, iCol);
    PackedByteArray arr;
    arr.resize(size);
    memcpy(arr.ptrw(), data, size);
    return arr;
}

double SQLite3Statement::column_double(int iCol) {
    return _stmt ? sqlite3_column_double(_stmt, iCol) : 0.0;
}

int SQLite3Statement::column_int(int iCol) {
    return _stmt ? sqlite3_column_int(_stmt, iCol) : 0;
}

int64_t SQLite3Statement::column_int64(int iCol) {
    return _stmt ? sqlite3_column_int64(_stmt, iCol) : 0;
}

String SQLite3Statement::column_text(int iCol) {
    return _stmt ? String((const char*)sqlite3_column_text(_stmt, iCol)) : String();
}

String SQLite3Statement::column_text16(int iCol) {
    return _stmt ? String((const char16_t*)sqlite3_column_text16(_stmt, iCol)) : String();
}

Variant SQLite3Statement::column_value(int iCol) {
    if (!_stmt) return Variant();
    sqlite3_value* val = sqlite3_column_value(_stmt, iCol);
    int type = sqlite3_value_type(val);
    switch (type) {
        case SQLITE_NULL:
            return Variant();
        case SQLITE_INTEGER:
            return Variant((int64_t)sqlite3_value_int64(val));
        case SQLITE_FLOAT:
            return Variant(sqlite3_value_double(val));
        case SQLITE_TEXT:
            return Variant(String((const char*)sqlite3_value_text(val)));
        case SQLITE_BLOB: {
            int size = sqlite3_value_bytes(val);
            PackedByteArray arr;
            arr.resize(size);
            memcpy(arr.ptrw(), sqlite3_value_blob(val), size);
            return Variant(arr);
        }
        default:
            return Variant();
    }
}

int SQLite3Statement::column_bytes(int iCol) {
    return _stmt ? sqlite3_column_bytes(_stmt, iCol) : 0;
}

int SQLite3Statement::column_bytes16(int iCol) {
    return _stmt ? sqlite3_column_bytes16(_stmt, iCol) : 0;
}

int SQLite3Statement::column_type(int iCol) {
    return _stmt ? sqlite3_column_type(_stmt, iCol) : SQLITE_NULL;
}

int SQLite3Statement::column_count() {
    return _stmt ? sqlite3_column_count(_stmt) : 0;
}

String SQLite3Statement::column_name(int N) {
    return _stmt ? String(sqlite3_column_name(_stmt, N)) : String();
}

String SQLite3Statement::column_name16(int N) {
    return _stmt ? String((const char16_t*)sqlite3_column_name16(_stmt, N)) : String();
}

String SQLite3Statement::column_database_name(int N) {
    return _stmt ? String(sqlite3_column_database_name(_stmt, N)) : String();
}

String SQLite3Statement::column_database_name16(int N) {
    return _stmt ? String((const char16_t*)sqlite3_column_database_name16(_stmt, N)) : String();
}

String SQLite3Statement::column_table_name(int N) {
    return _stmt ? String(sqlite3_column_table_name(_stmt, N)) : String();
}

String SQLite3Statement::column_table_name16(int N) {
    return _stmt ? String((const char16_t*)sqlite3_column_table_name16(_stmt, N)) : String();
}

String SQLite3Statement::column_origin_name(int N) {
    return _stmt ? String(sqlite3_column_origin_name(_stmt, N)) : String();
}

String SQLite3Statement::column_origin_name16(int N) {
    return _stmt ? String((const char16_t*)sqlite3_column_origin_name16(_stmt, N)) : String();
}

String SQLite3Statement::column_decltype(int N) {
    return _stmt ? String(sqlite3_column_decltype(_stmt, N)) : String();
}

String SQLite3Statement::column_decltype16(int N) {
    return _stmt ? String((const char16_t*)sqlite3_column_decltype16(_stmt, N)) : String();
}

int SQLite3Statement::stmt_status(int op, bool reset) {
    return _stmt ? sqlite3_stmt_status(_stmt, op, reset ? 1 : 0) : 0;
}



void SQLite3Statement::_bind_methods() {
    ClassDB::bind_method(D_METHOD("sql"), &SQLite3Statement::sql);
    ClassDB::bind_method(D_METHOD("expanded_sql"), &SQLite3Statement::expanded_sql);

    ClassDB::bind_method(D_METHOD("readonly"), &SQLite3Statement::readonly);
    ClassDB::bind_method(D_METHOD("isexplain"), &SQLite3Statement::isexplain);
    ClassDB::bind_method(D_METHOD("explain", "eMode"), &SQLite3Statement::explain);
    ClassDB::bind_method(D_METHOD("busy"), &SQLite3Statement::busy);

    ClassDB::bind_method(D_METHOD("bind_blob", "index", "value", "transient"), &SQLite3Statement::bind_blob, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("bind_blob64", "index", "value", "transient"), &SQLite3Statement::bind_blob64, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("bind_double", "index", "value"), &SQLite3Statement::bind_double);
    ClassDB::bind_method(D_METHOD("bind_int", "index", "value"), &SQLite3Statement::bind_int);
    ClassDB::bind_method(D_METHOD("bind_int64", "index", "value"), &SQLite3Statement::bind_int64);
    ClassDB::bind_method(D_METHOD("bind_null", "index"), &SQLite3Statement::bind_null);
    ClassDB::bind_method(D_METHOD("bind_text", "index", "value", "transient"), &SQLite3Statement::bind_text, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("bind_text16", "index", "value", "transient"), &SQLite3Statement::bind_text16, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("bind_text64", "index", "value", "transient", "encoding"), &SQLite3Statement::bind_text64, DEFVAL(true), DEFVAL(SQLITE_UTF8));
    ClassDB::bind_method(D_METHOD("bind_value", "index", "value"), &SQLite3Statement::bind_value);

    ClassDB::bind_method(D_METHOD("bind_zeroblob", "index", "n"), &SQLite3Statement::bind_zeroblob);
    ClassDB::bind_method(D_METHOD("bind_zeroblob64", "index", "n"), &SQLite3Statement::bind_zeroblob64);

    ClassDB::bind_method(D_METHOD("bind_parameter_count"), &SQLite3Statement::bind_parameter_count);
    ClassDB::bind_method(D_METHOD("bind_parameter_name", "index"), &SQLite3Statement::bind_parameter_name);
    ClassDB::bind_method(D_METHOD("bind_parameter_index", "name"), &SQLite3Statement::bind_parameter_index);
    ClassDB::bind_method(D_METHOD("clear_bindings"), &SQLite3Statement::clear_bindings);

    ClassDB::bind_method(D_METHOD("step"), &SQLite3Statement::step);
    ClassDB::bind_method(D_METHOD("data_count"), &SQLite3Statement::data_count);
    ClassDB::bind_method(D_METHOD("finalize"), &SQLite3Statement::finalize);
    ClassDB::bind_method(D_METHOD("reset"), &SQLite3Statement::reset);

    ClassDB::bind_method(D_METHOD("column_blob", "iCol"), &SQLite3Statement::column_blob);
    ClassDB::bind_method(D_METHOD("column_double", "iCol"), &SQLite3Statement::column_double);
    ClassDB::bind_method(D_METHOD("column_int", "iCol"), &SQLite3Statement::column_int);
    ClassDB::bind_method(D_METHOD("column_int64", "iCol"), &SQLite3Statement::column_int64);
    ClassDB::bind_method(D_METHOD("column_text", "iCol"), &SQLite3Statement::column_text);
    ClassDB::bind_method(D_METHOD("column_text16", "iCol"), &SQLite3Statement::column_text16);
    ClassDB::bind_method(D_METHOD("column_value", "iCol"), &SQLite3Statement::column_value);
    ClassDB::bind_method(D_METHOD("column_bytes", "iCol"), &SQLite3Statement::column_bytes);
    ClassDB::bind_method(D_METHOD("column_bytes16", "iCol"), &SQLite3Statement::column_bytes16);
    ClassDB::bind_method(D_METHOD("column_type", "iCol"), &SQLite3Statement::column_type);
    ClassDB::bind_method(D_METHOD("column_count"), &SQLite3Statement::column_count);

    ClassDB::bind_method(D_METHOD("column_name", "N"), &SQLite3Statement::column_name);
    ClassDB::bind_method(D_METHOD("column_name16", "N"), &SQLite3Statement::column_name16);
    ClassDB::bind_method(D_METHOD("column_database_name", "N"), &SQLite3Statement::column_database_name);
    ClassDB::bind_method(D_METHOD("column_database_name16", "N"), &SQLite3Statement::column_database_name16);
    ClassDB::bind_method(D_METHOD("column_table_name", "N"), &SQLite3Statement::column_table_name);
    ClassDB::bind_method(D_METHOD("column_table_name16", "N"), &SQLite3Statement::column_table_name16);
    ClassDB::bind_method(D_METHOD("column_origin_name", "N"), &SQLite3Statement::column_origin_name);
    ClassDB::bind_method(D_METHOD("column_origin_name16", "N"), &SQLite3Statement::column_origin_name16);
    ClassDB::bind_method(D_METHOD("column_decltype", "N"), &SQLite3Statement::column_decltype);
    ClassDB::bind_method(D_METHOD("column_decltype16", "N"), &SQLite3Statement::column_decltype16);

    ClassDB::bind_method(D_METHOD("stmt_status", "op", "reset"), &SQLite3Statement::stmt_status, DEFVAL(false));
}