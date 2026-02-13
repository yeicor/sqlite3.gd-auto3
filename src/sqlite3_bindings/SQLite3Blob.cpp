#include "SQLite3Blob.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

SQLite3Blob::SQLite3Blob() : _blob(nullptr) {}

SQLite3Blob::SQLite3Blob(sqlite3_blob* blob) : _blob(blob) {}

SQLite3Blob::~SQLite3Blob() {
    if (_blob) {
        sqlite3_blob_close(_blob);
    }
}

int SQLite3Blob::reopen(int64_t iRow) {
    return _blob ? sqlite3_blob_reopen(_blob, iRow) : SQLITE_MISUSE;
}

PackedByteArray SQLite3Blob::read(int n, int offset) {
    PackedByteArray buffer;
    if (!_blob) return buffer;
    buffer.resize(n);
    int rc = sqlite3_blob_read(_blob, buffer.ptrw(), n, offset);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Blob read error: ", String(sqlite3_errstr(rc)));
        buffer.clear();
    }
    return buffer;
}

int SQLite3Blob::write(const PackedByteArray& buffer, int offset) {
    return _blob ? sqlite3_blob_write(_blob, buffer.ptr(), buffer.size(), offset) : SQLITE_MISUSE;
}

int SQLite3Blob::bytes() {
    return _blob ? sqlite3_blob_bytes(_blob) : 0;
}

int SQLite3Blob::close() {
    if (!_blob) return SQLITE_MISUSE;
    int rc = sqlite3_blob_close(_blob);
    _blob = nullptr;
    return rc;
}

void SQLite3Blob::_bind_methods() {
    ClassDB::bind_method(D_METHOD("reopen", "iRow"), &SQLite3Blob::reopen);
    ClassDB::bind_method(D_METHOD("read", "n", "offset"), &SQLite3Blob::read);
    ClassDB::bind_method(D_METHOD("write", "buffer", "offset"), &SQLite3Blob::write);
    ClassDB::bind_method(D_METHOD("bytes"), &SQLite3Blob::bytes);
    ClassDB::bind_method(D_METHOD("close"), &SQLite3Blob::close);
}