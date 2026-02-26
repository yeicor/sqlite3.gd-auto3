# Copy upstream port files except portfile.cmake
file(COPY "${VCPKG_ROOT_DIR}/ports/icu/" DESTINATION "${CMAKE_CURRENT_LIST_DIR}" PATTERN "portfile.cmake" EXCLUDE)

# Patch upstream portfile.cmake
file(READ "${VCPKG_ROOT_DIR}/ports/icu/portfile.cmake" upstream_content)
if(VCPKG_TARGET_IS_EMSCRIPTEN)
    string(REPLACE "vcpkg_list(SET CONFIGURE_OPTIONS)" "vcpkg_list(SET CONFIGURE_OPTIONS)\nvcpkg_list(APPEND CONFIGURE_OPTIONS \"--with-pic\" \"CFLAGS=-fPIC -matomics -mbulk-memory\" \"CXXFLAGS=-fPIC -matomics -mbulk-memory\" \"LDFLAGS=-fPIC -matomics -mbulk-memory\")" upstream_content "${upstream_content}")
endif()
if(VCPKG_TARGET_IS_MINGW)
    string(REPLACE "if(CMAKE_HOST_WIN32 AND VCPKG_TARGET_IS_MINGW AND NOT HOST_TRIPLET MATCHES \"mingw\")" "if(CMAKE_HOST_WIN32 AND VCPKG_TARGET_IS_MINGW AND NOT HOST_TRIPLET MATCHES \"mingw\")\n    if(\"${VCPKG_TARGET_ARCHITECTURE}\" STREQUAL \"x64\")\n        set(_mingw_arch \"x86_64\")\n    else()\n        set(_mingw_arch \"${VCPKG_TARGET_ARCHITECTURE}\")\n    endif()\n    vcpkg_list(APPEND CONFIGURE_OPTIONS \"--host=\$x86_64-pc-mingw32\" \"--build=\${_mingw_arch}-pc-mingw32\")\nendif()\nif(CMAKE_HOST_WIN32 AND VCPKG_TARGET_IS_MINGW AND NOT HOST_TRIPLET MATCHES \"mingw\" AND \"${VCPKG_TARGET_ARCHITECTURE}\" STREQUAL \"x64\")" upstream_content "${upstream_content}")
endif()
if(VCPKG_TARGET_IS_IOS)
    string(REPLACE "vcpkg_list(SET CONFIGURE_OPTIONS)" "vcpkg_list(SET CONFIGURE_OPTIONS)\nvcpkg_list(APPEND CONFIGURE_OPTIONS \"--host=arm64-apple-darwin\")" upstream_content "${upstream_content}")
endif()

file(WRITE "${CMAKE_CURRENT_LIST_DIR}/portfile_upstream_patched.cmake" "${upstream_content}")

# Include the patched portfile.cmake
include("${CMAKE_CURRENT_LIST_DIR}/portfile_upstream_patched.cmake")
