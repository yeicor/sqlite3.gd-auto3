# Copy upstream port files except portfile.cmake
file(COPY "${VCPKG_ROOT_DIR}/ports/sqlite3/" DESTINATION "${CMAKE_CURRENT_LIST_DIR}" PATTERN "portfile.cmake" EXCLUDE)

# Patch upstream CMakeLists.txt to add emscripten args for threads mode
file(READ "${VCPKG_ROOT_DIR}/ports/sqlite3/CMakeLists.txt" upstream_content)
string(APPEND upstream_content "if(EMSCRIPTEN)\ntarget_compile_options(sqlite3 PRIVATE -matomics -mbulk-memory -fPIC)\nendif()")
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt" "${modified_content}")

# Include the upstream portfile.cmake
include("${VCPKG_ROOT_DIR}/ports/sqlite3/portfile.cmake")
