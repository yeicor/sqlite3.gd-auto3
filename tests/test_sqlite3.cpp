#include <gtest/gtest.h>

#include <sqlite3.h>

#include "SQLite3.h"
#include "SQLite3Database.h"

TEST(SQLite3GlobalFunctions, LibraryVersion) {
    // Test that binding calls the correct raw function
    EXPECT_EQ(SQLite3::libversion_number(), sqlite3_libversion_number());
}

TEST(SQLite3GlobalFunctions, ThreadSafety) {
    EXPECT_EQ(SQLite3::threadsafe(), sqlite3_threadsafe());
}

TEST(SQLite3GlobalFunctions, MemoryFunctions) {
    // Memory used may vary, but at least test the binding calls
    int64_t binding_used = SQLite3::memory_used();
    sqlite3_int64 raw_used = sqlite3_memory_used();
    EXPECT_EQ(binding_used, raw_used);
}