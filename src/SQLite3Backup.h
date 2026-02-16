#ifndef _SQLITE3_BACKUP_H
#define _SQLITE3_BACKUP_H

/**
 * SQLite3Backup.h
 *
 * Godot GDExtension wrapper for SQLite3 backup operations.
 *
 * This class wraps sqlite3_backup* and provides methods for database backup.
 *
 * Original SQLite3 header: <sqlite3.h>
 *
 * This file is part of SQLite3.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>

#include <sqlite3.h>

using namespace godot;

/**
 * SQLite3Backup
 *
 * Wrapper class for sqlite3_backup.
 */
class SQLite3Backup : public RefCounted {
    GDCLASS(SQLite3Backup, RefCounted);

protected:
    static void _bind_methods();

private:
    sqlite3_backup* _backup;

public:
    // Constructors
    SQLite3Backup();
    SQLite3Backup(sqlite3_backup* backup);
    virtual ~SQLite3Backup();

    // Backup operations
    int step(int nPage);
    int remaining();
    int pagecount();
    int finish();
};

#endif // _SQLITE3_BACKUP_H