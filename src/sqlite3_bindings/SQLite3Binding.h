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
    static int libversion_number();
    static String libversion();
    static String sourceid();

    // Initialization and shutdown
    static int initialize();
    static int shutdown();
    static int os_init();
    static int os_end();

    // Configuration
    static int config(int op, Variant args = Variant());  // Simplified, varargs hard

    // Memory management
    static void *malloc(int size);
    static void *malloc64(uint64_t size);
    static void *realloc(void *ptr, int size);
    static void *realloc64(void *ptr, uint64_t size);
    static void free_ptr(void *ptr);
    static int64_t msize(void *ptr);
    static int64_t memory_used();
    static int64_t memory_highwater(bool reset);

    // Status
    static Array status(int op, bool reset = false);
    static Array status64(int op, bool reset = false);

    // Randomness
    static void randomness(int N, PackedByteArray &buffer);

    // String formatting
    static String mprintf(const String &format);  // Simplified

    // Error strings
    static String errstr(int errcode);

    // Threading
    static int threadsafe();

    // Status
    static int status(int op, int &current, int &highwater, bool reset = false);

    // Keyword functions
    static int keyword_count();
    static bool keyword_name(int index, String &name, int &length);
    static bool keyword_check(const String &name, int length = -1);

    // Sleep
    static int sleep(int ms);

    // Temporary directory (get/set)
    static String get_temp_directory();
    static void set_temp_directory(const String &dir);

    // Data directory
    static String get_data_directory();
    static void set_data_directory(const String &dir);

    // Enable shared cache
    static int enable_shared_cache(int enable);

    // Release memory
    static int release_memory(int bytes);

    // Soft heap limit
    static int64_t soft_heap_limit64(int64_t limit);
    static int64_t hard_heap_limit64(int64_t limit);

    // Test control
    static int test_control(int op, Variant args = Variant());

    // Logging
    static void log(int errcode, const String &message);

    // Compile option
    static bool compileoption_used(const String &opt);
    static String compileoption_get(int N);



};

#endif // _SQLITE3_H