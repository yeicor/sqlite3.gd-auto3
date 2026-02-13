#ifndef _SQLITE3_DATABASE_H
#define _SQLITE3_DATABASE_H

/**
 * SQLite3Database.h
 *
 * Godot GDExtension wrapper for SQLite3 database handle.
 *
 * This class wraps sqlite3* and provides methods for database operations.
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

class SQLite3Statement;
class SQLite3ResultSet;
class SQLite3Backup;
class SQLite3Blob;

/**
 * SQLite3Database
 *
 * Wrapper class for sqlite3 database handle.
 */
class SQLite3Database : public RefCounted {
    GDCLASS(SQLite3Database, RefCounted);

protected:
    static void _bind_methods();

private:
    sqlite3* _db;

public:
    // Constructors
    SQLite3Database();
    SQLite3Database(sqlite3* db);
    virtual ~SQLite3Database();

    // Open database (static factory)
    static Ref<SQLite3Database> open(const String& filename, int flags = 0, const String& vfs = String());
    static Ref<SQLite3Database> open_v2(const String& filename, int flags = 0, const String& vfs = String());

    // Close
    int close();
    int close_v2();

    // Execute SQL
    int exec(const String& sql);  // Simplified, no callback

    // Error handling
    int errcode();
    int extended_errcode();
    String errmsg();
    String errmsg16();
    String errstr(int errcode);
    int error_offset();

    // Last insert rowid
    int64_t last_insert_rowid();
    void set_last_insert_rowid(int64_t rowid);

    // Changes
    int changes();
    int64_t changes64();
    int total_changes();
    int64_t total_changes64();

    // Interrupt
    void interrupt();
    bool is_interrupted();

    // Complete
    static bool complete(const String& sql);

    // Busy handler/timeout
    int busy_handler(Callable handler);  // Simplified
    int busy_timeout(int ms);
    int setlk_timeout(int ms, int flags);

    // Prepare statement
    Ref<SQLite3Statement> prepare(const String& sql, int nByte = -1);
    Ref<SQLite3Statement> prepare_v2(const String& sql, int nByte = -1);
    Ref<SQLite3Statement> prepare_v3(const String& sql, int nByte = -1, unsigned int prepFlags = 0);

    // Get table (simplified)
    Array get_table(const String& sql);  // Returns array of arrays

    // Configuration
    int db_config(int op, Variant args = Variant());

    // Autocommit
    bool get_autocommit();

    // DB handle from statement
    static Ref<SQLite3Database> db_handle(Ref<SQLite3Statement> stmt);

    // DB name/filename
    String db_name(int N);
    String db_filename(const String& zDbName = String());
    bool db_readonly(const String& zDbName = String());

    // Transaction state
    int txn_state(const String& zSchema = String());

    // Next statement
    Ref<SQLite3Statement> next_stmt(Ref<SQLite3Statement> pStmt);

    // Hooks
    void commit_hook(Callable hook);
    void rollback_hook(Callable hook);
    void update_hook(Callable hook);

    // Autovacuum pages
    int autovacuum_pages(Callable callback);

    // Enable load extension
    int enable_load_extension(int onoff);

    // Load extension
    int load_extension(const String& zFile, const String& zProc = String());

    // Auto extension
    static int auto_extension(Callable xEntryPoint);
    static int cancel_auto_extension(Callable xEntryPoint);
    static void reset_auto_extension();

    // Create module
    int create_module(const String& zName, Variant pModule);  // Simplified

    // Declare VTab
    int declare_vtab(const String& zSQL);

    // Overload function
    int overload_function(const String& zFuncName, int nArg);

    // Blob operations
    Ref<SQLite3Blob> blob_open(const String& zDb, const String& zTable, const String& zColumn, int64_t iRow, int flags);

    // VFS
    int file_control(const String& zDbName, int op, Variant pArg = Variant());

    // Status
    Array db_status(int op, bool resetFlg = false);

    // Cache flush
    int db_cacheflush();

    // System errno
    int system_errno();

    // Serialize/Deserialize
    Array serialize(const String& zSchema, unsigned int mFlags = 0);
    int deserialize(const String& zSchema, const PackedByteArray& pData, int64_t szDb, int64_t szBuf, unsigned int mFlags);

    // Rtree callbacks
    int rtree_geometry_callback(const String& zGeom, Callable xGeom, Variant pContext);
    int rtree_query_callback(const String& zQueryFunc, Callable xQueryFunc, Variant pContext);

    // Limit
    int limit(int id, int newVal);

    // Table column metadata
    Dictionary table_column_metadata(const String& zDbName, const String& zTableName, const String& zColumnName);

    // Release memory
    int db_release_memory();

    int64_t soft_heap_limit64(int64_t N);

    // WAL checkpoint
    int wal_checkpoint(const String& zDb = String());
    Array wal_checkpoint_v2(const String& zDb = String(), int eMode = 0);

    // Query with iterator
    Ref<SQLite3ResultSet> query(const String& sql);

    // Backup
    Ref<SQLite3Backup> backup_init(const String& zDestName, Ref<SQLite3Database> destDb, const String& zSrcName);

   // Internal access
    sqlite3* get_db() const { return _db; }
    void set_db(sqlite3* db) { _db = db; }
};

#endif // _SQLITE3_DATABASE_H