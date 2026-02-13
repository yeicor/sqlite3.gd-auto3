# Test suite for sqlite3.gd

cmake_minimum_required(VERSION 3.14)

# Option to enable/disable tests
option(BUILD_TESTING "Build the testing tree" ON)

if(NOT BUILD_TESTING)
    return()
endif()

# Skip tests when cross-compiling, as the test binary cannot run on the host
if(CMAKE_CROSSCOMPILING)
    return()
endif()

# Skip tests for Emscripten (WASM) as GTest requires exceptions which are disabled
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    return()
endif()

# Find GTest
find_package(GTest CONFIG REQUIRED)

# Find SQLite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)

# The main library target is ${PROJECT_NAME}

# Collect all test source files
file(GLOB TEST_SOURCES "*.cpp")

# Create test executable
add_executable(sqlite3_gd_tests ${TEST_SOURCES})

target_include_directories(sqlite3_gd_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/sqlite3_bindings
    ${CMAKE_CURRENT_SOURCE_DIR}/../godot-cpp/include
    ${CMAKE_BINARY_DIR}/godot-cpp/gen/include
    ${unofficial-sqlite3_INCLUDE_DIRS}
)

target_link_libraries(sqlite3_gd_tests PRIVATE
    ${PROJECT_NAME}
    godot-cpp
    unofficial::sqlite3::sqlite3
    GTest::gtest_main
)

# Set C++ standard
target_compile_features(sqlite3_gd_tests PRIVATE cxx_std_17)

# GTest main is provided by GTest::gtest_main

# Add as CTest test
include(CTest)
add_test(NAME sqlite3_gd_tests COMMAND sqlite3_gd_tests)

# Custom target to run tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS sqlite3_gd_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running sqlite3.gd tests..."
)
