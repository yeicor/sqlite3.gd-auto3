# Copy upstream port files except portfile.cmake
file(COPY "${VCPKG_ROOT_DIR}/ports/sqlite3/" DESTINATION "${CMAKE_CURRENT_LIST_DIR}" PATTERN "portfile.cmake" EXCLUDE)

# Patch upstream portfile.cmake to use the files from this directory instead of the original ones
file(READ "${VCPKG_ROOT_DIR}/ports/sqlite3/portfile.cmake" upstream_content)
string(REPLACE "\${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_CURRENT_LIST_DIR}" upstream_content "${upstream_content}")
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/portfile_upstream_patched.cmake" "${upstream_content}")

# Patch upstream portfile.cmake to add emscripten args for threads mode and PIC always on
file(READ "${VCPKG_ROOT_DIR}/ports/sqlite3/CMakeLists.txt" upstream_content)
if(VCPKG_TARGET_IS_EMSCRIPTEN)
    string(APPEND upstream_content "\ntarget_compile_options(sqlite3 PRIVATE -fPIC -matomics -mbulk-memory)")
endif()
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt" "${upstream_content}")

# Include the upstream portfile.cmake
include("${CMAKE_CURRENT_LIST_DIR}/portfile_upstream_patched.cmake")
