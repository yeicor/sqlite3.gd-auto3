MESSAGE(STATUS "Using toolchain file: "
    "${CMAKE_TOOLCHAIN_FILE}")
cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(sqlite3.gd CXX)

# Option to build tests (disabled by default for vcpkg builds)
option(BUILD_TESTING "Build the test suite" OFF)

# Syntax-only mode for fast builds (only checks syntax, does not link or
# install)
if(SYNTAX_ONLY)
    message(STATUS "SYNTAX_ONLY mode enabled: Only checking syntax, "
        "skipping linking and installation.")
endif()

# Enable testing
if(BUILD_TESTING)
    enable_testing()
endif()

# Check that both CMAKE_BUILD_TYPE and GODOTCPP_TARGET are set correctly
# (always required)
if (NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND
    NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(FATAL_ERROR "CMAKE_BUILD_TYPE must be either 'Release' or 'Debug'. "
        "Current value: ${CMAKE_BUILD_TYPE}")
endif ()
if (NOT GODOTCPP_TARGET STREQUAL "template_release" AND
    NOT GODOTCPP_TARGET STREQUAL "template_debug")
    message(FATAL_ERROR "GODOTCPP_TARGET must be either 'template_release' or "
        "'template_debug'. Current value: ${GODOTCPP_TARGET}")
endif ()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}, "
    "GODOTCPP_TARGET: ${GODOTCPP_TARGET}")
# No-op when CMAKE_BUILD_TYPE does not match the GODOTCPP_TARGET
if (CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT GODOTCPP_TARGET STREQUAL "template_release")
    message(WARNING "CMAKE_BUILD_TYPE is 'Release' but GODOTCPP_TARGET is not 'template_release'. Building anyway.")
    # No-op installation
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT GODOTCPP_TARGET STREQUAL "template_debug")
    message(WARNING "CMAKE_BUILD_TYPE is 'Debug' but GODOTCPP_TARGET is not 'template_debug'. Building anyway.")
    # No-op installation
endif ()

# Auto-enable ccache-like functionality if available since we are rebuilding godot-cpp and the bindings frequently
# If ccache, sccache or similar is found, use it as the CMAKE_{C,CXX}_COMPILER_LAUNCHER
find_program(CCACHE_EXECUTABLE ccache)
if(CCACHE_EXECUTABLE)
    message(STATUS "Using ccache as compiler launcher: "
        "${CCACHE_EXECUTABLE}")
    set(CMAKE_C_COMPILER_LAUNCHER
        ${CCACHE_EXECUTABLE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE})
endif()
file(GLOB HELPER_PATHS_WIN_SCCACHE_GITHUB_ACTIONS "C:/hostedtoolcache/windows/sccache/*/*")
message("Looking for sccache in: "
    "${HELPER_PATHS_WIN_SCCACHE_GITHUB_ACTIONS}")
find_program(SCCACHE_EXECUTABLE sccache
    PATHS ${HELPER_PATHS_WIN_SCCACHE_GITHUB_ACTIONS})
if(SCCACHE_EXECUTABLE)
    message(STATUS "Using sccache as compiler launcher: "
        "${SCCACHE_EXECUTABLE}")
    set(CMAKE_C_COMPILER_LAUNCHER
        ${SCCACHE_EXECUTABLE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_EXECUTABLE})
endif()
if(NOT CCACHE_EXECUTABLE AND NOT SCCACHE_EXECUTABLE)
    message(STATUS "No ccache or sccache found, not using a compiler "
        "launcher")
endif()

# Auto-force full LTO in release builds if available
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_SUPPORT_OUTPUT)
    if(IPO_SUPPORTED)
        message(STATUS "IPO (LTO) is supported, enabling it for Release builds")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO (LTO) is not supported: "
            "${IPO_SUPPORT_OUTPUT}")
    endif()
else()
    message(STATUS "IPO (LTO) is not enabled for Debug "
        "builds")
endif()

# Godot C++ bindings
set(GODOTCPP_DISABLE_EXCEPTIONS OFF)
# FIXME: Godot tries to find cocoa for all apple platforms, open an issue in godot-cpp to fix this
if (APPLE AND IOS)
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp/cmake/macos.cmake"
        "${MACOS_CMAKE_CONTENTS}")
    string(REPLACE "if(APPLE)" "if(FALSE)" MACOS_CMAKE_CONTENTS "${MACOS_CMAKE_CONTENTS}")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp/cmake/macos.cmake"
        "${MACOS_CMAKE_CONTENTS}")
    message(WARNING "Modified godot-cpp macos.cmake to not include cocoa for iOS "
        "builds:\n${MACOS_CMAKE_CONTENTS}")
endif ()
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp")

# Dependencies
find_package(unofficial-sqlite3 CONFIG REQUIRED)

# Define the documentation sources (the godot-cpp cmake version does not
# work?)
if(GODOTCPP_TARGET MATCHES "editor|template_debug")
    find_package(Python3 3.4 REQUIRED) # pathlib should be present
    file(GLOB_RECURSE DOC_XML LIST_DIRECTORIES NO CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/doc_classes/*.xml"
        "${PROJECT_SOURCE_DIR}/doc_classes/**/*.xml")
    set(DOC_GEN_FILE "${CMAKE_CURRENT_BINARY_DIR}/gen/doc_data.cpp")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gen")
    set(PYTHON_LIST "'${DOC_XML}'")
    list(JOIN PYTHON_LIST "','" PYTHON_LIST)
    add_custom_command(
    OUTPUT ${DOC_GEN_FILE}
    COMMAND ${Python3_EXECUTABLE} "-c"
        "from doc_source_generator import generate_doc_source; generate_doc_source('${DOC_GEN_FILE}', [${PYTHON_LIST}])"
    VERBATIM
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp
    DEPENDS ${DOC_XML})
    add_custom_target(generate_doc_data DEPENDS ${DOC_GEN_FILE})
endif()

# Define the shared library target
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/**/*.cpp")
if(SYNTAX_ONLY)
    # Syntax-only mode: Compile sources as OBJECT library, do not link or install
    add_library(${PROJECT_NAME}_syntax_only OBJECT ${SOURCES} ${DOC_GEN_FILE})
    target_link_libraries(${PROJECT_NAME}_syntax_only PRIVATE godot-cpp unofficial::sqlite3::sqlite3)
    target_compile_options(${PROJECT_NAME}_syntax_only PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-fsyntax-only>
        $<$<CXX_COMPILER_ID:MSVC>:/Zs>
    )
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/") # No-op installation
else()
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${DOC_GEN_FILE})
    target_link_libraries(${PROJECT_NAME} PRIVATE godot-cpp unofficial::sqlite3::sqlite3)

    # Install the complete bindings to the demo project with the correct suffix
    get_target_property(GODOTCPP_SUFFIX godot-cpp GODOTCPP_SUFFIX)
    set(OUTPUT_NAME "${PROJECT_NAME}${GODOTCPP_SUFFIX}")
    if (NOT DEFINED _INSTALL_TO_ROOT)
        set(_INSTALL_TO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
    endif ()
    set(INSTALL_FOLDER "${_INSTALL_TO_ROOT}/demo/addons/${PROJECT_NAME}")
    message("Will install shared library to: ${INSTALL_FOLDER}")
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${OUTPUT_NAME}")
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "${INSTALL_FOLDER}")
    # FIXME: Try harder to install to the proper directory on windows???
endif()

# Add tests subdirectory if testing is enabled
if(BUILD_TESTING AND NOT SYNTAX_ONLY)
    add_subdirectory(tests)
endif()

# Copy compile_commands.json to source dir for IDE support
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/compile_commands.json
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMENT "Copying compile_commands.json to source directory"
    )
    add_custom_target(copy_compile_commands
        DEPENDS ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif()
