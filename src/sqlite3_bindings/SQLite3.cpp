#include "SQLite3.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

int SQLite3::libversion_number() {
    return sqlite3_libversion_number();
}

String SQLite3::libversion() {
    return String(sqlite3_libversion());
}

String SQLite3::sourceid() {
    return String(sqlite3_sourceid());
}

int SQLite3::initialize() {
    return sqlite3_initialize();
}

int SQLite3::shutdown() {
    return sqlite3_shutdown();
}

int SQLite3::os_init() {
    return sqlite3_os_init();
}

int SQLite3::os_end() {
    return sqlite3_os_end();
}

int SQLite3::config(int op, Variant args) {
    // Simplified: varargs not handled, just call with op
    return sqlite3_config(op);
}

void *SQLite3::malloc(int size) {
    return sqlite3_malloc(size);
}

void *SQLite3::malloc64(uint64_t size) {
    return sqlite3_malloc64(size);
}

void *SQLite3::realloc(void *ptr, int size) {
    return sqlite3_realloc(ptr, size);
}

void *SQLite3::realloc64(void *ptr, uint64_t size) {
    return sqlite3_realloc64(ptr, size);
}

void SQLite3::free_ptr(void *ptr) {
    sqlite3_free(ptr);
}

int64_t SQLite3::msize(void *ptr) {
    return (int64_t)sqlite3_msize(ptr);
}

int64_t SQLite3::memory_used() {
    return sqlite3_memory_used();
}

int64_t SQLite3::memory_highwater(bool reset) {
    return sqlite3_memory_highwater(reset ? 1 : 0);
}

Array SQLite3::status(int op, bool reset) {
    int current, highwater;
    int rc = sqlite3_status(op, &current, &highwater, reset ? 1 : 0);
    Array arr;
    arr.append(rc);
    arr.append(current);
    arr.append(highwater);
    return arr;
}

Array SQLite3::status64(int op, bool reset) {
    sqlite3_int64 current, highwater;
    int rc = sqlite3_status64(op, &current, &highwater, reset ? 1 : 0);
    Array arr;
    arr.append(rc);
    arr.append((int64_t)current);
    arr.append((int64_t)highwater);
    return arr;
}

void SQLite3::randomness(int N, PackedByteArray &buffer) {
    buffer.resize(N);
    sqlite3_randomness(N, buffer.ptrw());
}

String SQLite3::mprintf(const String &format) {
    char *result = sqlite3_mprintf(format.utf8().get_data());
    String s = String(result);
    sqlite3_free(result);
    return s;
}

String SQLite3::errstr(int errcode) {
    return String(sqlite3_errstr(errcode));
}

int SQLite3::threadsafe() {
    return sqlite3_threadsafe();
}

int SQLite3::status(int op, int &current, int &highwater, bool reset) {
    return sqlite3_status(op, &current, &highwater, reset ? 1 : 0);
}

int SQLite3::keyword_count() {
    return sqlite3_keyword_count();
}

bool SQLite3::keyword_name(int index, String &name, int &length) {
    const char *zName;
    int len;
    int rc = sqlite3_keyword_name(index, &zName, &len);
    if (rc == SQLITE_OK) {
        name = String(zName);
        length = len;
    }
    return rc == SQLITE_OK;
}

bool SQLite3::keyword_check(const String &name, int length) {
    if (length < 0) length = name.length();
    return sqlite3_keyword_check(name.utf8().get_data(), length);
}

int SQLite3::sleep(int ms) {
    return sqlite3_sleep(ms);
}

String SQLite3::get_temp_directory() {
    return String(sqlite3_temp_directory);
}

void SQLite3::set_temp_directory(const String &dir) {
    if (sqlite3_temp_directory) sqlite3_free(sqlite3_temp_directory);
    sqlite3_temp_directory = sqlite3_mprintf("%s", dir.utf8().get_data());
}

String SQLite3::get_data_directory() {
    return String(sqlite3_data_directory);
}

void SQLite3::set_data_directory(const String &dir) {
    if (sqlite3_data_directory) sqlite3_free(sqlite3_data_directory);
    sqlite3_data_directory = sqlite3_mprintf("%s", dir.utf8().get_data());
}

int SQLite3::enable_shared_cache(int enable) {
    return sqlite3_enable_shared_cache(enable);
}

int SQLite3::release_memory(int bytes) {
    return sqlite3_release_memory(bytes);
}

int64_t SQLite3::soft_heap_limit64(int64_t limit) {
    return sqlite3_soft_heap_limit64(limit);
}

int64_t SQLite3::hard_heap_limit64(int64_t limit) {
    return sqlite3_hard_heap_limit64(limit);
}

int SQLite3::test_control(int op, Variant args) {
    // Simplified
    return sqlite3_test_control(op);
}

void SQLite3::log(int errcode, const String &message) {
    sqlite3_log(errcode, "%s", message.utf8().get_data());
}

bool SQLite3::compileoption_used(const String &opt) {
    return sqlite3_compileoption_used(opt.utf8().get_data());
}

String SQLite3::compileoption_get(int N) {
    const char *opt = sqlite3_compileoption_get(N);
    return opt ? String(opt) : String();
}



void SQLite3::_bind_methods() {
    ClassDB::bind_static_method("SQLite3", D_METHOD("libversion_number"), &SQLite3::libversion_number);
    ClassDB::bind_static_method("SQLite3", D_METHOD("libversion"), &SQLite3::libversion);
    ClassDB::bind_static_method("SQLite3", D_METHOD("sourceid"), &SQLite3::sourceid);
    ClassDB::bind_static_method("SQLite3", D_METHOD("initialize"), &SQLite3::initialize);
    ClassDB::bind_static_method("SQLite3", D_METHOD("shutdown"), &SQLite3::shutdown);
    ClassDB::bind_static_method("SQLite3", D_METHOD("os_init"), &SQLite3::os_init);
    ClassDB::bind_static_method("SQLite3", D_METHOD("os_end"), &SQLite3::os_end);
    ClassDB::bind_static_method("SQLite3", D_METHOD("config", "op", "args"), &SQLite3::config, DEFVAL(Variant()));

    // ClassDB::bind_static_method("SQLite3", D_METHOD("malloc", "size"), &SQLite3::malloc);
    // ClassDB::bind_static_method("SQLite3", D_METHOD("malloc64", "size"), &SQLite3::malloc64);
    // ClassDB::bind_static_method("SQLite3", D_METHOD("realloc", "ptr", "size"), &SQLite3::realloc);
    // ClassDB::bind_static_method("SQLite3", D_METHOD("realloc64", "ptr", "size"), &SQLite3::realloc64);
    // ClassDB::bind_static_method("SQLite3", D_METHOD("free_ptr", "ptr"), &SQLite3::free_ptr);
    // ClassDB::bind_static_method("SQLite3", D_METHOD("msize", "ptr"), &SQLite3::msize);
    ClassDB::bind_static_method("SQLite3", D_METHOD("memory_used"), &SQLite3::memory_used);
    ClassDB::bind_static_method("SQLite3", D_METHOD("memory_highwater", "reset"), &SQLite3::memory_highwater, DEFVAL(false));
    // ClassDB::bind_static_method("SQLite3", D_METHOD("status", "op", "reset"), &SQLite3::status, DEFVAL(false));
    // ClassDB::bind_static_method("SQLite3", D_METHOD("status64", "op", "reset"), &SQLite3::status64, DEFVAL(false));
    // ClassDB::bind_static_method("SQLite3", D_METHOD("randomness", "N", "buffer"), &SQLite3::randomness);
    ClassDB::bind_static_method("SQLite3", D_METHOD("mprintf", "format"), &SQLite3::mprintf);
    ClassDB::bind_static_method("SQLite3", D_METHOD("errstr", "errcode"), &SQLite3::errstr);
    ClassDB::bind_static_method("SQLite3", D_METHOD("threadsafe"), &SQLite3::threadsafe);
    // ClassDB::bind_static_method("SQLite3", D_METHOD("status", "op", "current", "highwater", "reset"), &SQLite3::status, DEFVAL(false));
    ClassDB::bind_static_method("SQLite3", D_METHOD("keyword_count"), &SQLite3::keyword_count);
    // ClassDB::bind_static_method("SQLite3", D_METHOD("keyword_name", "index", "name", "length"), &SQLite3::keyword_name);
    ClassDB::bind_static_method("SQLite3", D_METHOD("keyword_check", "name", "length"), &SQLite3::keyword_check, DEFVAL(-1));
    ClassDB::bind_static_method("SQLite3", D_METHOD("sleep", "ms"), &SQLite3::sleep);
    ClassDB::bind_static_method("SQLite3", D_METHOD("get_temp_directory"), &SQLite3::get_temp_directory);
    ClassDB::bind_static_method("SQLite3", D_METHOD("set_temp_directory", "dir"), &SQLite3::set_temp_directory);
    ClassDB::bind_static_method("SQLite3", D_METHOD("get_data_directory"), &SQLite3::get_data_directory);
    ClassDB::bind_static_method("SQLite3", D_METHOD("set_data_directory", "dir"), &SQLite3::set_data_directory);
    ClassDB::bind_static_method("SQLite3", D_METHOD("enable_shared_cache", "enable"), &SQLite3::enable_shared_cache);
    ClassDB::bind_static_method("SQLite3", D_METHOD("release_memory", "bytes"), &SQLite3::release_memory);
    ClassDB::bind_static_method("SQLite3", D_METHOD("soft_heap_limit64", "limit"), &SQLite3::soft_heap_limit64);
    ClassDB::bind_static_method("SQLite3", D_METHOD("hard_heap_limit64", "limit"), &SQLite3::hard_heap_limit64);
    // ClassDB::bind_static_method("SQLite3", D_METHOD("test_control", "op", "args"), &SQLite3::test_control, DEFVAL(Variant()));
    // ClassDB::bind_static_method("SQLite3", D_METHOD("log", "errcode", "message"), &SQLite3::log);
    ClassDB::bind_static_method("SQLite3", D_METHOD("compileoption_used", "opt"), &SQLite3::compileoption_used);
    ClassDB::bind_static_method("SQLite3", D_METHOD("compileoption_get", "N"), &SQLite3::compileoption_get);
}