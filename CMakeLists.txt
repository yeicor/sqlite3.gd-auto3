cmake_minimum_required(VERSION 3.25)

message(STATUS "Using toolchain file: " "${CMAKE_TOOLCHAIN_FILE}")

# Auto-enable sccache or ccache if available since we are rebuilding godot-cpp and the bindings frequently
set(CACHE_PROGRAMS sccache ccache)
set(CACHE_EXECUTABLE "")
foreach(CACHE_PROG IN LISTS CACHE_PROGRAMS)
    find_program(${CACHE_PROG}_EXECUTABLE ${CACHE_PROG})
    if(${CACHE_PROG}_EXECUTABLE)
        set(CACHE_EXECUTABLE ${${CACHE_PROG}_EXECUTABLE})
        message(STATUS "Using ${CACHE_PROG} as compiler launcher: ${CACHE_EXECUTABLE}")
        break()
    endif()
endforeach()
if(CACHE_EXECUTABLE)
    set(CMAKE_C_COMPILER_LAUNCHER ${CACHE_EXECUTABLE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CACHE_EXECUTABLE})
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>")
    cmake_policy(SET CMP0141 NEW)
else()
    message(STATUS "Neither sccache nor ccache found, not using a compiler launcher")
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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(sqlite3.gd CXX)  # TODO: Change to your project name, e.g., project(my_extension CXX)

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
# (it can make sense, but it is rare enough to not be worth building them by default)
if (CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT GODOTCPP_TARGET STREQUAL "template_release")
    message(WARNING "CMAKE_BUILD_TYPE is 'Release' but GODOTCPP_TARGET is not 'template_release'. Ignoring this build to speed up the build process.")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/") # No-op installation
    return()
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT GODOTCPP_TARGET STREQUAL "template_debug")
    message(WARNING "CMAKE_BUILD_TYPE is 'Debug' but GODOTCPP_TARGET is not 'template_debug'. Ignoring this build to speed up the build process.")
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/") # No-op installation
    return()
endif ()

# Fix CMAKE_OSX_SYSROOT for iOS to use full SDK path (overriden incorrectly by godot-cpp due to env SDKROOT)
set(ENV{SDKROOT} "${CMAKE_OSX_SYSROOT}")
set(GODOTCPP_DISABLE_EXCEPTIONS OFF)

# Godot C++ bindings
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp")


# Dependencies
find_package(unofficial-sqlite3 CONFIG REQUIRED)  # TODO: Replace with your dependencies

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
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    add_library(${PROJECT_NAME} MODULE ${SOURCES} ${DOC_GEN_FILE})
    target_link_options(${PROJECT_NAME} PRIVATE "-sSIDE_MODULE=1")
else()
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${DOC_GEN_FILE})
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE godot-cpp unofficial::sqlite3::sqlite3)

# Install the complete bindings to the demo project with the correct suffix
get_target_property(GODOTCPP_SUFFIX godot-cpp GODOTCPP_SUFFIX)
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
string(FIND "${GODOTCPP_SUFFIX}" "${GODOTCPP_TARGET}" _target_pos)
if(_target_pos EQUAL -1)
    message(FATAL_ERROR "GODOTCPP_TARGET '${GODOTCPP_TARGET}' not found in GODOTCPP_SUFFIX '${GODOTCPP_SUFFIX}'")
endif()
math(EXPR _insert_pos "${_target_pos} + string(LENGTH \"${GODOTCPP_TARGET}\")")
string(SUBSTRING "${GODOTCPP_SUFFIX}" 0 ${_insert_pos} _prefix)
string(SUBSTRING "${GODOTCPP_SUFFIX}" ${_insert_pos} -1 _suffix)
set(GODOTCPP_SUFFIX "${_prefix}.${BUILD_TYPE_LOWER}${_suffix}")
set(OUTPUT_NAME "gdext${GODOTCPP_SUFFIX}")
if (MINGW) # Avoid name collision for both supported windows compilers (msvc and mingw)
    set(OUTPUT_NAME "gdext${GODOTCPP_SUFFIX}.mingw")
endif()
set(INSTALL_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/demo/addons/${PROJECT_NAME}")
message("Will install shared library to: ${INSTALL_FOLDER}")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${OUTPUT_NAME}")
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib") # Unix-like behavior on windows too: lib*.dll
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "${INSTALL_FOLDER}")
