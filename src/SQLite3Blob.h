#ifndef _SQLITE3_BLOB_H
#define _SQLITE3_BLOB_H

/**
 * SQLite3Blob.h
 *
 * Godot GDExtension wrapper for SQLite3 blob handle.
 *
 * This class wraps sqlite3_blob* and provides methods for incremental blob I/O.
 *
 * Original SQLite3 header: <sqlite3.h>
 *
 * This file is part of SQLite3.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <sqlite3.h>

using namespace godot;

/**
 * SQLite3Blob
 *
 * Wrapper class for sqlite3_blob.
 */
class SQLite3Blob : public RefCounted {
    GDCLASS(SQLite3Blob, RefCounted);

protected:
    static void _bind_methods();

private:
    sqlite3_blob* _blob;

public:
    // Constructors
    SQLite3Blob();
    SQLite3Blob(sqlite3_blob* blob);
    virtual ~SQLite3Blob();

    // Blob operations
    int reopen(int64_t iRow);
    PackedByteArray read(int n, int offset);
    int write(const PackedByteArray& buffer, int offset);
    int bytes();
    int close();
};

#endif // _SQLITE3_BLOB_H