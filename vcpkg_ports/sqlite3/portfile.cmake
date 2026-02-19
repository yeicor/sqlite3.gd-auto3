# XXX: Enable required emscripten args for threads mode
set(content_to_add "if(EMSCRIPTEN)\ntarget_compile_options(sqlite3 PRIVATE -matomics -mbulk-memory -fPIC)\nendif()")
set(file_to_edit "${VCPKG_ROOT_DIR}/ports/sqlite3/CMakeLists.txt")
file(READ "${file_to_edit}" content)
string(FIND "${content}" "${content_to_add}" index_of_content)
if(index_of_content EQUAL -1)
    file(APPEND "${file_to_edit}" "\n${content_to_add}\n")
endif()

# Need access to some port files
file(COPY "${VCPKG_ROOT_DIR}/ports/sqlite3/" DESTINATION "${CMAKE_CURRENT_LIST_DIR}" PATTERN "portfile.cmake" EXCLUDE)

include("${VCPKG_ROOT_DIR}/ports/sqlite3/portfile.cmake")
