#ifndef _SQLITE3_STATEMENT_H
#define _SQLITE3_STATEMENT_H

/**
 * SQLite3Statement.h
 *
 * Godot GDExtension wrapper for SQLite3 statement handle.
 *
 * This class wraps sqlite3_stmt* and provides methods for prepared statements.
 *
 * Original SQLite3 header: <sqlite3.h>
 *
 * This file is part of SQLite3.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <sqlite3.h>

using namespace godot;

/**
 * SQLite3Statement
 *
 * Wrapper class for sqlite3_stmt.
 */
class SQLite3Statement : public RefCounted {
    GDCLASS(SQLite3Statement, RefCounted);

protected:
    static void _bind_methods();

private:
    sqlite3_stmt* _stmt;

public:
    // Constructors
    SQLite3Statement();
    SQLite3Statement(sqlite3_stmt* stmt);
    virtual ~SQLite3Statement();

    // SQL and readonly
    String sql();
    String expanded_sql();
    bool readonly();
    bool isexplain();
    int explain(int eMode);
    bool busy();

    // Bind methods
    int bind_blob(int index, const PackedByteArray& value, bool transient = true);
    int bind_blob64(int index, const PackedByteArray& value, bool transient = true);
    int bind_double(int index, double value);
    int bind_int(int index, int value);
    int bind_int64(int index, int64_t value);
    int bind_null(int index);
    int bind_text(int index, const String& value, bool transient = true);
    int bind_text16(int index, const String& value, bool transient = true);
    int bind_text64(int index, const String& value, bool transient = true, int encoding = SQLITE_UTF8);
    int bind_value(int index, const Variant& value);
    int bind_zeroblob(int index, int n);
    int bind_zeroblob64(int index, int64_t n);

    // Bind parameter info
    int bind_parameter_count();
    String bind_parameter_name(int index);
    int bind_parameter_index(const String& name);
    int clear_bindings();

    // Step and finalize
    int step();
    int data_count();
    int finalize();
    int reset();

    // Column access
    PackedByteArray column_blob(int iCol);
    double column_double(int iCol);
    int column_int(int iCol);
    int64_t column_int64(int iCol);
    String column_text(int iCol);
    String column_text16(int iCol);
    Variant column_value(int iCol);
    int column_bytes(int iCol);
    int column_bytes16(int iCol);
    int column_type(int iCol);
    int column_count();

    // Column names
    String column_name(int N);
    String column_name16(int N);
    String column_database_name(int N);
    String column_database_name16(int N);
    String column_table_name(int N);
    String column_table_name16(int N);
    String column_origin_name(int N);
    String column_origin_name16(int N);
    String column_decltype(int N);
    String column_decltype16(int N);

    // Status
    int stmt_status(int op, bool reset = false);

    // Internal access
    sqlite3_stmt* get_stmt() const { return _stmt; }
    void set_stmt(sqlite3_stmt* stmt) { _stmt = stmt; }
};

#endif // _SQLITE3_STATEMENT_H