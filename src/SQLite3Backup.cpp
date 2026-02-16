#include "SQLite3Backup.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

SQLite3Backup::SQLite3Backup() : _backup(nullptr) {}

SQLite3Backup::SQLite3Backup(sqlite3_backup* backup) : _backup(backup) {}

SQLite3Backup::~SQLite3Backup() {
    if (_backup) {
        sqlite3_backup_finish(_backup);
    }
}

int SQLite3Backup::step(int nPage) {
    return _backup ? sqlite3_backup_step(_backup, nPage) : SQLITE_MISUSE;
}

int SQLite3Backup::remaining() {
    return _backup ? sqlite3_backup_remaining(_backup) : 0;
}

int SQLite3Backup::pagecount() {
    return _backup ? sqlite3_backup_pagecount(_backup) : 0;
}

int SQLite3Backup::finish() {
    if (!_backup) return SQLITE_MISUSE;
    int rc = sqlite3_backup_finish(_backup);
    _backup = nullptr;
    return rc;
}

void SQLite3Backup::_bind_methods() {
    ClassDB::bind_method(D_METHOD("step", "nPage"), &SQLite3Backup::step);
    ClassDB::bind_method(D_METHOD("remaining"), &SQLite3Backup::remaining);
    ClassDB::bind_method(D_METHOD("pagecount"), &SQLite3Backup::pagecount);
    ClassDB::bind_method(D_METHOD("finish"), &SQLite3Backup::finish);
}