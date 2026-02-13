#include "SQLite3Database.h"
#include "SQLite3Statement.h"
#include "SQLite3ResultSet.h"
#include "SQLite3Backup.h"
#include "SQLite3Blob.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

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

void SQLite3Database::commit_hook(Callable hook) {
    // Simplified, no callback implementation
}

void SQLite3Database::rollback_hook(Callable hook) {
    // Simplified
}

void SQLite3Database::update_hook(Callable hook) {
    // Simplified
}

int SQLite3Database::autovacuum_pages(Callable callback) {
    // Simplified
    return SQLITE_MISUSE;
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
}