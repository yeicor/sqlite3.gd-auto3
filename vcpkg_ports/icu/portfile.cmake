# Copy upstream port files except portfile.cmake
file(COPY "${VCPKG_ROOT_DIR}/ports/icu/" DESTINATION "${CMAKE_CURRENT_LIST_DIR}" PATTERN "portfile.cmake" EXCLUDE)

# Patch upstream portfile.cmake
file(READ "${VCPKG_ROOT_DIR}/ports/icu/portfile.cmake" upstream_content)
if(VCPKG_TARGET_IS_EMSCRIPTEN)
    string(REPLACE "vcpkg_list(SET CONFIGURE_OPTIONS)" "vcpkg_list(SET CONFIGURE_OPTIONS)\nvcpkg_list(APPEND CONFIGURE_OPTIONS \"--with-pic\" \"CFLAGS=-fPIC -matomics -mbulk-memory\" \"CXXFLAGS=-fPIC -matomics -mbulk-memory\" \"LDFLAGS=-fPIC -matomics -mbulk-memory\")" upstream_content "${upstream_content}")
endif()
if(VCPKG_TARGET_IS_MINGW)
    string(REPLACE "if(CMAKE_HOST_WIN32 AND VCPKG_TARGET_IS_MINGW AND NOT HOST_TRIPLET MATCHES \"mingw\")" "if(FALSE AND CMAKE_HOST_WIN32 AND VCPKG_TARGET_IS_MINGW AND NOT HOST_TRIPLET MATCHES \"mingw\")" upstream_content "${upstream_content}")
endif()
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/portfile_upstream_patched.cmake" "${upstream_content}")

# Include the patched portfile.cmake
include("${CMAKE_CURRENT_LIST_DIR}/portfile_upstream_patched.cmake")
