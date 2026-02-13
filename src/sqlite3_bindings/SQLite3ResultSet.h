#ifndef _SQLITE3_RESULT_SET_H
#define _SQLITE3_RESULT_SET_H

/**
 * SQLite3ResultSet.h
 *
 * Godot GDExtension wrapper for SQLite3 result set iteration.
 *
 * This class wraps sqlite3_stmt* and provides iterator-like access to query results.
 *
 * Original SQLite3 header: <sqlite3.h>
 *
 * This file is part of SQLite3.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <sqlite3.h>

using namespace godot;

/**
 * SQLite3ResultSet
 *
 * Wrapper class for iterating over SQLite3 query results.
 */
class SQLite3ResultSet : public RefCounted {
    GDCLASS(SQLite3ResultSet, RefCounted);

protected:
    static void _bind_methods();

private:
    sqlite3_stmt* _stmt;
    bool _done;

public:
    // Constructors
    SQLite3ResultSet();
    SQLite3ResultSet(sqlite3_stmt* stmt);
    virtual ~SQLite3ResultSet();

    // Iteration
    bool next();
    Dictionary current_row();
    Array column_names();
    int column_count();

    // Close
    void close();
};

#endif // _SQLITE3_RESULT_SET_H