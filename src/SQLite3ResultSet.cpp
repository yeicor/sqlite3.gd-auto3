

#include "SQLite3ResultSet.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

SQLite3ResultSet::SQLite3ResultSet() : _stmt(nullptr), _done(true) {}

SQLite3ResultSet::SQLite3ResultSet(sqlite3_stmt* stmt) : _stmt(stmt), _done(false) {}

SQLite3ResultSet::~SQLite3ResultSet() {
    if (_stmt) {
        sqlite3_finalize(_stmt);
    }
}

bool SQLite3ResultSet::next() {
    if (!_stmt || _done) return false;
    int rc = sqlite3_step(_stmt);
    if (rc == SQLITE_ROW) {
        return true;
    } else if (rc == SQLITE_DONE) {
        _done = true;
        return false;
    } else {
        UtilityFunctions::printerr("Step error: ", String(sqlite3_errstr(rc)));
        _done = true;
        return false;
    }
}

Dictionary SQLite3ResultSet::current_row() {
    Dictionary row;
    if (!_stmt || _done) return row;
    int cols = sqlite3_column_count(_stmt);
    for (int i = 0; i < cols; ++i) {
        String name = String(sqlite3_column_name(_stmt, i));
        Variant value;
        int type = sqlite3_column_type(_stmt, i);
        switch (type) {
            case SQLITE_NULL:
                value = Variant();
                break;
            case SQLITE_INTEGER:
                value = Variant((int64_t)sqlite3_column_int64(_stmt, i));
                break;
            case SQLITE_FLOAT:
                value = Variant(sqlite3_column_double(_stmt, i));
                break;
            case SQLITE_TEXT:
                value = Variant(String((const char*)sqlite3_column_text(_stmt, i)));
                break;
            case SQLITE_BLOB: {
                int size = sqlite3_column_bytes(_stmt, i);
                PackedByteArray arr;
                arr.resize(size);
                memcpy(arr.ptrw(), sqlite3_column_blob(_stmt, i), size);
                value = Variant(arr);
                break;
            }
        }
        row[name] = value;
    }
    return row;
}

Array SQLite3ResultSet::column_names() {
    Array names;
    if (!_stmt) return names;
    int cols = sqlite3_column_count(_stmt);
    for (int i = 0; i < cols; ++i) {
        names.append(String(sqlite3_column_name(_stmt, i)));
    }
    return names;
}

int SQLite3ResultSet::column_count() {
    return _stmt ? sqlite3_column_count(_stmt) : 0;
}

void SQLite3ResultSet::close() {
    if (_stmt) {
        sqlite3_finalize(_stmt);
        _stmt = nullptr;
    }
    _done = true;
}

void SQLite3ResultSet::_bind_methods() {
    ClassDB::bind_method(D_METHOD("next"), &SQLite3ResultSet::next);
    ClassDB::bind_method(D_METHOD("current_row"), &SQLite3ResultSet::current_row);
    ClassDB::bind_method(D_METHOD("column_names"), &SQLite3ResultSet::column_names);
    ClassDB::bind_method(D_METHOD("column_count"), &SQLite3ResultSet::column_count);
    ClassDB::bind_method(D_METHOD("close"), &SQLite3ResultSet::close);
}