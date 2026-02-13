#ifndef _SQLITE3_H
#define _SQLITE3_H

/**
 * SQLite3.h
 *
 * Godot GDExtension wrapper for SQLite3 global functions.
 *
 * This class provides static methods for SQLite3 library initialization,
 * version information, and other global operations.
 *
 * Original SQLite3 header: <sqlite3.h>
 *
 * This file is part of SQLite3.gd bindings.
 */

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include <sqlite3.h>
#include <cstdarg>

using namespace godot;

/**
 * SQLite3
 *
 * Wrapper class for SQLite3 global functions.
 */
class SQLite3 : public Object {
    GDCLASS(SQLite3, Object);

protected:
    static void _bind_methods();

public:
    // Library version functions
    static GDE_EXPORT int libversion_number();
    static GDE_EXPORT String libversion();
    static GDE_EXPORT String sourceid();

    // Initialization and shutdown
    static GDE_EXPORT int initialize();
    static GDE_EXPORT int shutdown();
    static GDE_EXPORT int os_init();
    static GDE_EXPORT int os_end();

    // Configuration
    static GDE_EXPORT int config(int op, Variant args = Variant());  // Simplified, varargs hard

    // Memory management
    static GDE_EXPORT void *malloc(int size);
    static GDE_EXPORT void *malloc64(uint64_t size);
    static GDE_EXPORT void *realloc(void *ptr, int size);
    static GDE_EXPORT void *realloc64(void *ptr, uint64_t size);
    static GDE_EXPORT void free_ptr(void *ptr);
    static GDE_EXPORT int64_t msize(void *ptr);
    static GDE_EXPORT int64_t memory_used();
    static GDE_EXPORT int64_t memory_highwater(bool reset);

    // Status
    static GDE_EXPORT Array status(int op, bool reset = false);
    static GDE_EXPORT Array status64(int op, bool reset = false);

    // Randomness
    static GDE_EXPORT void randomness(int N, PackedByteArray &buffer);

    // String formatting
    static GDE_EXPORT String mprintf(const String &format);  // Simplified

    // Error strings
    static GDE_EXPORT String errstr(int errcode);

    // Threading
    static GDE_EXPORT int threadsafe();

    // Status
    static GDE_EXPORT int status(int op, int &current, int &highwater, bool reset = false);

    // Keyword functions
    static GDE_EXPORT int keyword_count();
    static GDE_EXPORT bool keyword_name(int index, String &name, int &length);
    static GDE_EXPORT bool keyword_check(const String &name, int length = -1);

    // Sleep
    static GDE_EXPORT int sleep(int ms);

    // Temporary directory (get/set)
    static GDE_EXPORT String get_temp_directory();
    static GDE_EXPORT void set_temp_directory(const String &dir);

    // Data directory
    static GDE_EXPORT String get_data_directory();
    static GDE_EXPORT void set_data_directory(const String &dir);

    // Enable shared cache
    static GDE_EXPORT int enable_shared_cache(int enable);

    // Release memory
    static GDE_EXPORT int release_memory(int bytes);

    // Soft heap limit
    static GDE_EXPORT int64_t soft_heap_limit64(int64_t limit);
    static GDE_EXPORT int64_t hard_heap_limit64(int64_t limit);

    // Test control
    static GDE_EXPORT int test_control(int op, Variant args = Variant());

    // Logging
    static GDE_EXPORT void log(int errcode, const String &message);

    // Compile option
    static GDE_EXPORT bool compileoption_used(const String &opt);
    static GDE_EXPORT String compileoption_get(int N);



};

#endif // _SQLITE3_H