#include "SQLite3Database.h"
#include "SQLite3Statement.h"
#include "SQLite3ResultSet.h"
#include "SQLite3Backup.h"
#include "SQLite3Blob.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// Callback functions
static int busy_handler_callback(void* user_data, int count) {
    SQLite3Database* db = static_cast<SQLite3Database*>(user_data);
    if (db->_busy_handler.is_valid()) {
        Variant result = db->_busy_handler.call(count);
        return result.operator int();
    }
    return 0;
}

static int commit_hook_callback(void* user_data) {
    SQLite3Database* db = static_cast<SQLite3Database*>(user_data);
    if (db->_commit_hook.is_valid()) {
        Variant result = db->_commit_hook.call();
        return result.operator int();
    }
    return 0;
}

static void rollback_hook_callback(void* user_data) {
    SQLite3Database* db = static_cast<SQLite3Database*>(user_data);
    if (db->_rollback_hook.is_valid()) {
        db->_rollback_hook.call();
    }
}

static void update_hook_callback(void* user_data, int type, const char* db, const char* table, sqlite3_int64 rowid) {
    SQLite3Database* db_obj = static_cast<SQLite3Database*>(user_data);
    if (db_obj->_update_hook.is_valid()) {
        db_obj->_update_hook.call(type, String(db), String(table), Variant((int64_t)rowid));
    }
}

static unsigned int autovacuum_pages_callback(void* user_data, const char* db_name, unsigned int nPage, unsigned int nFree, unsigned int rc) {
    SQLite3Database* db = static_cast<SQLite3Database*>(user_data);
    if (db->_autovacuum_callback.is_valid()) {
        Variant result = db->_autovacuum_callback.call(String(db_name), nPage, nFree, rc);
        return (unsigned int)result.operator int();
    }
    return SQLITE_OK;
}

SQLite3Database::SQLite3Database() : _db(nullptr) {}

SQLite3Database::SQLite3Database(sqlite3* db) : _db(db) {}

SQLite3Database::~SQLite3Database() {
    if (_db) {
        sqlite3_close_v2(_db);
        _db = nullptr;
    }
}

Ref<SQLite3Database> SQLite3Database::open(const String& filename, int flags, const String& vfs) {
    sqlite3* db;
    int rc = sqlite3_open(filename.utf8().get_data(), &db);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Failed to open database: ", String(sqlite3_errmsg(db)));
        sqlite3_close(db);
        return Ref<SQLite3Database>();
    }
    return Ref<SQLite3Database>(memnew(SQLite3Database(db)));
}

Ref<SQLite3Database> SQLite3Database::open_v2(const String& filename, int flags, const String& vfs) {
    sqlite3* db;
    int rc = sqlite3_open_v2(filename.utf8().get_data(), &db, flags, vfs.is_empty() ? nullptr : vfs.utf8().get_data());
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Failed to open database: ", String(sqlite3_errmsg(db)));
        sqlite3_close(db);
        return Ref<SQLite3Database>();
    }
    return Ref<SQLite3Database>(memnew(SQLite3Database(db)));
}

int SQLite3Database::close() {
    if (!_db) return SQLITE_OK;
    int rc = sqlite3_close(_db);
    if (rc == SQLITE_OK) _db = nullptr;
    return rc;
}

int SQLite3Database::close_v2() {
    if (!_db) return SQLITE_OK;
    int rc = sqlite3_close_v2(_db);
    _db = nullptr;
    return rc;
}

int SQLite3Database::exec(const String& sql) {
    if (!_db) return SQLITE_MISUSE;
    char* errmsg;
    int rc = sqlite3_exec(_db, sql.utf8().get_data(), nullptr, nullptr, &errmsg);
    if (errmsg) {
        UtilityFunctions::printerr("SQL exec error: ", String(errmsg));
        sqlite3_free(errmsg);
    }
    return rc;
}

int SQLite3Database::errcode() {
    return _db ? sqlite3_errcode(_db) : SQLITE_MISUSE;
}

int SQLite3Database::extended_errcode() {
    return _db ? sqlite3_extended_errcode(_db) : SQLITE_MISUSE;
}

String SQLite3Database::errmsg() {
    return _db ? String(sqlite3_errmsg(_db)) : String();
}

String SQLite3Database::errmsg16() {
    return _db ? String((const char16_t*)sqlite3_errmsg16(_db)) : String();
}

String SQLite3Database::errstr(int errcode) {
    return String(sqlite3_errstr(errcode));
}

int SQLite3Database::error_offset() {
    return _db ? sqlite3_error_offset(_db) : -1;
}

int64_t SQLite3Database::last_insert_rowid() {
    return _db ? sqlite3_last_insert_rowid(_db) : 0;
}

void SQLite3Database::set_last_insert_rowid(int64_t rowid) {
    if (_db) sqlite3_set_last_insert_rowid(_db, rowid);
}

int SQLite3Database::changes() {
    return _db ? sqlite3_changes(_db) : 0;
}

int64_t SQLite3Database::changes64() {
    return _db ? sqlite3_changes64(_db) : 0;
}

int SQLite3Database::total_changes() {
    return _db ? sqlite3_total_changes(_db) : 0;
}

int64_t SQLite3Database::total_changes64() {
    return _db ? sqlite3_total_changes64(_db) : 0;
}

void SQLite3Database::interrupt() {
    if (_db) sqlite3_interrupt(_db);
}

bool SQLite3Database::is_interrupted() {
    return _db ? sqlite3_is_interrupted(_db) : false;
}

bool SQLite3Database::complete(const String& sql) {
    return sqlite3_complete(sql.utf8().get_data());
}

int SQLite3Database::busy_timeout(int ms) {
    return _db ? sqlite3_busy_timeout(_db, ms) : SQLITE_MISUSE;
}

int SQLite3Database::setlk_timeout(int ms, int flags) {
    return _db ? sqlite3_setlk_timeout(_db, ms, flags) : SQLITE_MISUSE;
}

Ref<SQLite3Statement> SQLite3Database::prepare(const String& sql, int nByte) {
    if (!_db) return Ref<SQLite3Statement>();
    sqlite3_stmt* stmt;
    const char* tail;
    int rc = sqlite3_prepare(_db, sql.utf8().get_data(), nByte, &stmt, &tail);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Prepare error: ", errmsg());
        return Ref<SQLite3Statement>();
    }
    return Ref<SQLite3Statement>(memnew(SQLite3Statement(stmt)));
}

Ref<SQLite3Statement> SQLite3Database::prepare_v2(const String& sql, int nByte) {
    if (!_db) return Ref<SQLite3Statement>();
    sqlite3_stmt* stmt;
    const char* tail;
    int rc = sqlite3_prepare_v2(_db, sql.utf8().get_data(), nByte, &stmt, &tail);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Prepare v2 error: ", errmsg());
        return Ref<SQLite3Statement>();
    }
    return Ref<SQLite3Statement>(memnew(SQLite3Statement(stmt)));
}

Ref<SQLite3Statement> SQLite3Database::prepare_v3(const String& sql, int nByte, unsigned int prepFlags) {
    if (!_db) return Ref<SQLite3Statement>();
    sqlite3_stmt* stmt;
    const char* tail;
    int rc = sqlite3_prepare_v3(_db, sql.utf8().get_data(), nByte, prepFlags, &stmt, &tail);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Prepare v3 error: ", errmsg());
        return Ref<SQLite3Statement>();
    }
    return Ref<SQLite3Statement>(memnew(SQLite3Statement(stmt)));
}

Array SQLite3Database::get_table(const String& sql) {
    if (!_db) return Array();
    char** result;
    int nrow, ncol;
    char* errmsg;
    int rc = sqlite3_get_table(_db, sql.utf8().get_data(), &result, &nrow, &ncol, &errmsg);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Get table error: ", String(errmsg));
        sqlite3_free(errmsg);
        return Array();
    }
    Array table;
    for (int i = 0; i < nrow; ++i) {
        Array row;
        for (int j = 0; j < ncol; ++j) {
            row.append(String(result[(i+1)*ncol + j]));
        }
        table.append(row);
    }
    sqlite3_free_table(result);
    return table;
}

Ref<SQLite3ResultSet> SQLite3Database::query(const String& sql) {
    if (!_db) return Ref<SQLite3ResultSet>();
    sqlite3_stmt* stmt;
    const char* tail;
    int rc = sqlite3_prepare_v2(_db, sql.utf8().get_data(), -1, &stmt, &tail);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Prepare error: ", errmsg());
        return Ref<SQLite3ResultSet>();
    }
    return Ref<SQLite3ResultSet>(memnew(SQLite3ResultSet(stmt)));
}

Ref<SQLite3Backup> SQLite3Database::backup_init(const String& zDestName, Ref<SQLite3Database> destDb, const String& zSrcName) {
    if (!_db || !destDb.is_valid()) return Ref<SQLite3Backup>();
    sqlite3_backup* backup = sqlite3_backup_init(destDb->get_db(), zDestName.utf8().get_data(), _db, zSrcName.utf8().get_data());
    if (!backup) {
        UtilityFunctions::printerr("Backup init error");
        return Ref<SQLite3Backup>();
    }
    return Ref<SQLite3Backup>(memnew(SQLite3Backup(backup)));
}

int SQLite3Database::db_config(int op, Variant args) {
    // Simplified, varargs not handled
    return _db ? sqlite3_db_config(_db, op) : SQLITE_MISUSE;
}

bool SQLite3Database::get_autocommit() {
    return _db ? sqlite3_get_autocommit(_db) : false;
}

Ref<SQLite3Database> SQLite3Database::db_handle(Ref<SQLite3Statement> stmt) {
    if (!stmt.is_valid()) return Ref<SQLite3Database>();
    sqlite3* db = sqlite3_db_handle(stmt->get_stmt());
    return Ref<SQLite3Database>(memnew(SQLite3Database(db)));
}

String SQLite3Database::db_name(int N) {
    return _db ? String(sqlite3_db_name(_db, N)) : String();
}

String SQLite3Database::db_filename(const String& zDbName) {
    return _db ? String(sqlite3_db_filename(_db, zDbName.utf8().get_data())) : String();
}

bool SQLite3Database::db_readonly(const String& zDbName) {
    return _db ? sqlite3_db_readonly(_db, zDbName.utf8().get_data()) : false;
}

int SQLite3Database::txn_state(const String& zSchema) {
    return _db ? sqlite3_txn_state(_db, zSchema.utf8().get_data()) : -1;
}

Ref<SQLite3Statement> SQLite3Database::next_stmt(Ref<SQLite3Statement> pStmt) {
    if (!_db) return Ref<SQLite3Statement>();
    sqlite3_stmt* stmt = sqlite3_next_stmt(_db, pStmt.is_valid() ? pStmt->get_stmt() : nullptr);
    return stmt ? Ref<SQLite3Statement>(memnew(SQLite3Statement(stmt))) : Ref<SQLite3Statement>();
}

int SQLite3Database::busy_handler(Callable handler) {
    _busy_handler = handler;
    return sqlite3_busy_handler(_db, handler.is_valid() ? busy_handler_callback : nullptr, this);
}

void SQLite3Database::commit_hook(Callable hook) {
    _commit_hook = hook;
    sqlite3_commit_hook(_db, hook.is_valid() ? commit_hook_callback : nullptr, this);
}

void SQLite3Database::rollback_hook(Callable hook) {
    _rollback_hook = hook;
    sqlite3_rollback_hook(_db, hook.is_valid() ? rollback_hook_callback : nullptr, this);
}

void SQLite3Database::update_hook(Callable hook) {
    _update_hook = hook;
    sqlite3_update_hook(_db, hook.is_valid() ? update_hook_callback : nullptr, this);
}

int SQLite3Database::autovacuum_pages(Callable callback) {
    _autovacuum_callback = callback;
    return sqlite3_autovacuum_pages(_db, callback.is_valid() ? autovacuum_pages_callback : nullptr, this, nullptr);
}

int SQLite3Database::enable_load_extension(int onoff) {
    return _db ? sqlite3_enable_load_extension(_db, onoff) : SQLITE_MISUSE;
}

int SQLite3Database::load_extension(const String& zFile, const String& zProc) {
    return _db ? sqlite3_load_extension(_db, zFile.utf8().get_data(), zProc.is_empty() ? nullptr : zProc.utf8().get_data(), nullptr) : SQLITE_MISUSE;
}

int SQLite3Database::auto_extension(Callable xEntryPoint) {
    // Simplified
    return SQLITE_MISUSE;
}

int SQLite3Database::cancel_auto_extension(Callable xEntryPoint) {
    // Simplified
    return SQLITE_MISUSE;
}

void SQLite3Database::reset_auto_extension() {
    sqlite3_reset_auto_extension();
}

int SQLite3Database::create_module(const String& zName, Variant pModule) {
    // Simplified
    return SQLITE_MISUSE;
}

int SQLite3Database::declare_vtab(const String& zSQL) {
    return _db ? sqlite3_declare_vtab(_db, zSQL.utf8().get_data()) : SQLITE_MISUSE;
}

int SQLite3Database::overload_function(const String& zFuncName, int nArg) {
    return _db ? sqlite3_overload_function(_db, zFuncName.utf8().get_data(), nArg) : SQLITE_MISUSE;
}

Ref<SQLite3Blob> SQLite3Database::blob_open(const String& zDb, const String& zTable, const String& zColumn, int64_t iRow, int flags) {
    if (!_db) return Ref<SQLite3Blob>();
    sqlite3_blob* blob;
    int rc = sqlite3_blob_open(_db, zDb.utf8().get_data(), zTable.utf8().get_data(), zColumn.utf8().get_data(), iRow, flags, &blob);
    if (rc != SQLITE_OK) {
        UtilityFunctions::printerr("Blob open error: ", errmsg());
        return Ref<SQLite3Blob>();
    }
    return Ref<SQLite3Blob>(memnew(SQLite3Blob(blob)));
}

int SQLite3Database::file_control(const String& zDbName, int op, Variant pArg) {
    // Simplified
    return _db ? sqlite3_file_control(_db, zDbName.utf8().get_data(), op, nullptr) : SQLITE_MISUSE;
}

Array SQLite3Database::db_status(int op, bool resetFlg) {
    if (!_db) return Array();
    int pCur, pHiwtr;
    int rc = sqlite3_db_status(_db, op, &pCur, &pHiwtr, resetFlg);
    Array arr;
    arr.append(rc);
    arr.append(pCur);
    arr.append(pHiwtr);
    return arr;
}

int SQLite3Database::db_cacheflush() {
    return _db ? sqlite3_db_cacheflush(_db) : SQLITE_MISUSE;
}

int SQLite3Database::system_errno() {
    return _db ? sqlite3_system_errno(_db) : 0;
}

Array SQLite3Database::serialize(const String& zSchema, unsigned int mFlags) {
    if (!_db) return Array();
    sqlite3_int64 size;
    unsigned char* data = sqlite3_serialize(_db, zSchema.utf8().get_data(), &size, mFlags);
    if (!data) return Array();
    PackedByteArray result;
    result.resize(size);
    memcpy(result.ptrw(), data, size);
    sqlite3_free(data);
    Array arr;
    arr.append(result);
    arr.append((int64_t)size);
    return arr;
}

int SQLite3Database::deserialize(const String& zSchema, const PackedByteArray& pData, int64_t szDb, int64_t szBuf, unsigned int mFlags) {
    return _db ? sqlite3_deserialize(_db, zSchema.utf8().get_data(), (unsigned char*)pData.ptr(), szDb, szBuf, mFlags) : SQLITE_MISUSE;
}

int SQLite3Database::rtree_geometry_callback(const String& zGeom, Callable xGeom, Variant pContext) {
    // Simplified
    return SQLITE_MISUSE;
}

int SQLite3Database::rtree_query_callback(const String& zQueryFunc, Callable xQueryFunc, Variant pContext) {
    // Simplified
    return SQLITE_MISUSE;
}

int SQLite3Database::limit(int id, int newVal) {
    return _db ? sqlite3_limit(_db, id, newVal) : -1;
}

Dictionary SQLite3Database::table_column_metadata(const String& zDbName, const String& zTableName, const String& zColumnName) {
    Dictionary dict;
    if (!_db) {
        dict["error"] = SQLITE_MISUSE;
        return dict;
    }
    char const *pzDataType_c, *pzCollSeq_c;
    int pNotNull, pPrimaryKey, pAutoinc;
    int rc = sqlite3_table_column_metadata(_db, zDbName.utf8().get_data(), zTableName.utf8().get_data(), zColumnName.utf8().get_data(), &pzDataType_c, &pzCollSeq_c, &pNotNull, &pPrimaryKey, &pAutoinc);
    dict["error"] = rc;
    if (rc == SQLITE_OK) {
        dict["data_type"] = String(pzDataType_c);
        dict["collation_seq"] = String(pzCollSeq_c);
        dict["not_null"] = pNotNull;
        dict["primary_key"] = pPrimaryKey;
        dict["autoinc"] = pAutoinc;
    }
    return dict;
}

int SQLite3Database::db_release_memory() {
    return _db ? sqlite3_db_release_memory(_db) : 0;
}

int64_t SQLite3Database::soft_heap_limit64(int64_t N) {
    return sqlite3_soft_heap_limit64(N);
}

int SQLite3Database::wal_checkpoint(const String& zDb) {
    return _db ? sqlite3_wal_checkpoint(_db, zDb.is_empty() ? nullptr : zDb.utf8().get_data()) : SQLITE_MISUSE;
}

Array SQLite3Database::wal_checkpoint_v2(const String& zDb, int eMode) {
    Array result;
    if (!_db) {
        result.append(SQLITE_MISUSE);
        return result;
    }
    int pnLog, pnCkpt;
    int rc = sqlite3_wal_checkpoint_v2(_db, zDb.is_empty() ? nullptr : zDb.utf8().get_data(), eMode, &pnLog, &pnCkpt);
    result.append(rc);
    result.append(pnLog);
    result.append(pnCkpt);
    return result;
}

void SQLite3Database::_bind_methods() {
    ClassDB::bind_static_method("SQLite3Database", D_METHOD("open", "filename", "flags", "vfs"), &SQLite3Database::open, DEFVAL(0), DEFVAL(String()));
    ClassDB::bind_static_method("SQLite3Database", D_METHOD("open_v2", "filename", "flags", "vfs"), &SQLite3Database::open_v2, DEFVAL(0), DEFVAL(String()));
    ClassDB::bind_static_method("SQLite3Database", D_METHOD("db_handle", "stmt"), &SQLite3Database::db_handle);

    ClassDB::bind_method(D_METHOD("close"), &SQLite3Database::close);
    ClassDB::bind_method(D_METHOD("close_v2"), &SQLite3Database::close_v2);
    ClassDB::bind_method(D_METHOD("exec", "sql"), &SQLite3Database::exec);
    ClassDB::bind_method(D_METHOD("errcode"), &SQLite3Database::errcode);
    ClassDB::bind_method(D_METHOD("extended_errcode"), &SQLite3Database::extended_errcode);
    ClassDB::bind_method(D_METHOD("errmsg"), &SQLite3Database::errmsg);
    ClassDB::bind_method(D_METHOD("errmsg16"), &SQLite3Database::errmsg16);
    ClassDB::bind_method(D_METHOD("errstr", "errcode"), &SQLite3Database::errstr);
    ClassDB::bind_method(D_METHOD("error_offset"), &SQLite3Database::error_offset);
    ClassDB::bind_method(D_METHOD("last_insert_rowid"), &SQLite3Database::last_insert_rowid);
    ClassDB::bind_method(D_METHOD("set_last_insert_rowid", "rowid"), &SQLite3Database::set_last_insert_rowid);
    ClassDB::bind_method(D_METHOD("changes"), &SQLite3Database::changes);
    ClassDB::bind_method(D_METHOD("changes64"), &SQLite3Database::changes64);
    ClassDB::bind_method(D_METHOD("total_changes"), &SQLite3Database::total_changes);
    ClassDB::bind_method(D_METHOD("total_changes64"), &SQLite3Database::total_changes64);
    ClassDB::bind_method(D_METHOD("interrupt"), &SQLite3Database::interrupt);
    ClassDB::bind_method(D_METHOD("is_interrupted"), &SQLite3Database::is_interrupted);
    ClassDB::bind_static_method("SQLite3Database", D_METHOD("complete", "sql"), &SQLite3Database::complete);
    ClassDB::bind_method(D_METHOD("busy_timeout", "ms"), &SQLite3Database::busy_timeout);
    ClassDB::bind_method(D_METHOD("setlk_timeout", "ms", "flags"), &SQLite3Database::setlk_timeout);
    ClassDB::bind_method(D_METHOD("prepare", "sql", "nByte"), &SQLite3Database::prepare, DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("prepare_v2", "sql", "nByte"), &SQLite3Database::prepare_v2, DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("prepare_v3", "sql", "nByte", "prepFlags"), &SQLite3Database::prepare_v3, DEFVAL(-1), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("get_table", "sql"), &SQLite3Database::get_table);
    ClassDB::bind_method(D_METHOD("query", "sql"), &SQLite3Database::query);
    ClassDB::bind_method(D_METHOD("backup_init", "zDestName", "destDb", "zSrcName"), &SQLite3Database::backup_init);
    ClassDB::bind_method(D_METHOD("db_config", "op", "args"), &SQLite3Database::db_config, DEFVAL(Variant()));
    ClassDB::bind_method(D_METHOD("get_autocommit"), &SQLite3Database::get_autocommit);
    ClassDB::bind_method(D_METHOD("db_name", "N"), &SQLite3Database::db_name);
    ClassDB::bind_method(D_METHOD("db_filename", "zDbName"), &SQLite3Database::db_filename, DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("db_readonly", "zDbName"), &SQLite3Database::db_readonly, DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("txn_state", "zSchema"), &SQLite3Database::txn_state, DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("next_stmt", "pStmt"), &SQLite3Database::next_stmt);
    ClassDB::bind_method(D_METHOD("busy_handler", "handler"), &SQLite3Database::busy_handler);
    ClassDB::bind_method(D_METHOD("commit_hook", "hook"), &SQLite3Database::commit_hook);
    ClassDB::bind_method(D_METHOD("rollback_hook", "hook"), &SQLite3Database::rollback_hook);
    ClassDB::bind_method(D_METHOD("update_hook", "hook"), &SQLite3Database::update_hook);
    ClassDB::bind_method(D_METHOD("autovacuum_pages", "callback"), &SQLite3Database::autovacuum_pages);
    ClassDB::bind_method(D_METHOD("enable_load_extension", "onoff"), &SQLite3Database::enable_load_extension);
    ClassDB::bind_method(D_METHOD("load_extension", "zFile", "zProc"), &SQLite3Database::load_extension, DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("declare_vtab", "zSQL"), &SQLite3Database::declare_vtab);
    ClassDB::bind_method(D_METHOD("overload_function", "zFuncName", "nArg"), &SQLite3Database::overload_function);
    ClassDB::bind_method(D_METHOD("blob_open", "zDb", "zTable", "zColumn", "iRow", "flags"), &SQLite3Database::blob_open);
    ClassDB::bind_method(D_METHOD("file_control", "zDbName", "op", "pArg"), &SQLite3Database::file_control, DEFVAL(Variant()));
    ClassDB::bind_method(D_METHOD("db_status", "op", "resetFlg"), &SQLite3Database::db_status, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("db_cacheflush"), &SQLite3Database::db_cacheflush);
    ClassDB::bind_method(D_METHOD("system_errno"), &SQLite3Database::system_errno);
    ClassDB::bind_method(D_METHOD("serialize", "zSchema", "mFlags"), &SQLite3Database::serialize, DEFVAL(String()), DEFVAL(0));
    ClassDB::bind_method(D_METHOD("deserialize", "zSchema", "pData", "szDb", "szBuf", "mFlags"), &SQLite3Database::deserialize);
    ClassDB::bind_method(D_METHOD("limit", "id", "newVal"), &SQLite3Database::limit);
    ClassDB::bind_method(D_METHOD("table_column_metadata", "zDbName", "zTableName", "zColumnName"), &SQLite3Database::table_column_metadata);
    ClassDB::bind_method(D_METHOD("db_release_memory"), &SQLite3Database::db_release_memory);
    ClassDB::bind_method(D_METHOD("soft_heap_limit64", "N"), &SQLite3Database::soft_heap_limit64);
    ClassDB::bind_method(D_METHOD("wal_checkpoint", "zDb"), &SQLite3Database::wal_checkpoint, DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("wal_checkpoint_v2", "zDb", "eMode"), &SQLite3Database::wal_checkpoint_v2, DEFVAL(String()), DEFVAL(0));

    // Bind constants
    // Result codes
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OK"), SQLITE_OK);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_ERROR"), SQLITE_ERROR);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_INTERNAL"), SQLITE_INTERNAL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_PERM"), SQLITE_PERM);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_ABORT"), SQLITE_ABORT);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_BUSY"), SQLITE_BUSY);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LOCKED"), SQLITE_LOCKED);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_NOMEM"), SQLITE_NOMEM);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_READONLY"), SQLITE_READONLY);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_INTERRUPT"), SQLITE_INTERRUPT);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_IOERR"), SQLITE_IOERR);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_CORRUPT"), SQLITE_CORRUPT);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_NOTFOUND"), SQLITE_NOTFOUND);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_FULL"), SQLITE_FULL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_CANTOPEN"), SQLITE_CANTOPEN);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_PROTOCOL"), SQLITE_PROTOCOL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_EMPTY"), SQLITE_EMPTY);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_SCHEMA"), SQLITE_SCHEMA);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_TOOBIG"), SQLITE_TOOBIG);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_CONSTRAINT"), SQLITE_CONSTRAINT);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_MISMATCH"), SQLITE_MISMATCH);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_MISUSE"), SQLITE_MISUSE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_NOLFS"), SQLITE_NOLFS);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_AUTH"), SQLITE_AUTH);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_FORMAT"), SQLITE_FORMAT);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_RANGE"), SQLITE_RANGE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_NOTADB"), SQLITE_NOTADB);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_NOTICE"), SQLITE_NOTICE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_WARNING"), SQLITE_WARNING);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_ROW"), SQLITE_ROW);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_DONE"), SQLITE_DONE);

    // Open flags
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_READONLY"), SQLITE_OPEN_READONLY);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_READWRITE"), SQLITE_OPEN_READWRITE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_CREATE"), SQLITE_OPEN_CREATE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_DELETEONCLOSE"), SQLITE_OPEN_DELETEONCLOSE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_EXCLUSIVE"), SQLITE_OPEN_EXCLUSIVE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_AUTOPROXY"), SQLITE_OPEN_AUTOPROXY);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_URI"), SQLITE_OPEN_URI);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_MEMORY"), SQLITE_OPEN_MEMORY);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_MAIN_DB"), SQLITE_OPEN_MAIN_DB);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_TEMP_DB"), SQLITE_OPEN_TEMP_DB);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_TRANSIENT_DB"), SQLITE_OPEN_TRANSIENT_DB);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_MAIN_JOURNAL"), SQLITE_OPEN_MAIN_JOURNAL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_TEMP_JOURNAL"), SQLITE_OPEN_TEMP_JOURNAL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_SUBJOURNAL"), SQLITE_OPEN_SUBJOURNAL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_MASTER_JOURNAL"), SQLITE_OPEN_MASTER_JOURNAL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_NOMUTEX"), SQLITE_OPEN_NOMUTEX);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_FULLMUTEX"), SQLITE_OPEN_FULLMUTEX);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_SHAREDCACHE"), SQLITE_OPEN_SHAREDCACHE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_PRIVATECACHE"), SQLITE_OPEN_PRIVATECACHE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_WAL"), SQLITE_OPEN_WAL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_OPEN_NOFOLLOW"), SQLITE_OPEN_NOFOLLOW);

    // Data types
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_INTEGER"), SQLITE_INTEGER);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_FLOAT"), SQLITE_FLOAT);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_TEXT"), SQLITE_TEXT);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_BLOB"), SQLITE_BLOB);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_NULL"), SQLITE_NULL);

    // Other constants
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_TRANSIENT"), -1LL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_STATIC"), 0LL);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_UTF8"), SQLITE_UTF8);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_UTF16"), SQLITE_UTF16);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_UTF16BE"), SQLITE_UTF16BE);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_UTF16LE"), SQLITE_UTF16LE);

    // Limit constants
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_LENGTH"), SQLITE_LIMIT_LENGTH);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_SQL_LENGTH"), SQLITE_LIMIT_SQL_LENGTH);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_COLUMN"), SQLITE_LIMIT_COLUMN);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_EXPR_DEPTH"), SQLITE_LIMIT_EXPR_DEPTH);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_COMPOUND_SELECT"), SQLITE_LIMIT_COMPOUND_SELECT);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_VDBE_OP"), SQLITE_LIMIT_VDBE_OP);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_FUNCTION_ARG"), SQLITE_LIMIT_FUNCTION_ARG);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_ATTACHED"), SQLITE_LIMIT_ATTACHED);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_LIKE_PATTERN_LENGTH"), SQLITE_LIMIT_LIKE_PATTERN_LENGTH);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_VARIABLE_NUMBER"), SQLITE_LIMIT_VARIABLE_NUMBER);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_TRIGGER_DEPTH"), SQLITE_LIMIT_TRIGGER_DEPTH);
    ClassDB::bind_integer_constant(get_class_static(), StringName(), StringName("SQLITE_LIMIT_WORKER_THREADS"), SQLITE_LIMIT_WORKER_THREADS);
}