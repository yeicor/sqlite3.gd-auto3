# Copy upstream port files except portfile.cmake
file(COPY "${VCPKG_ROOT_DIR}/ports/icu/" DESTINATION "${CMAKE_CURRENT_LIST_DIR}" PATTERN "portfile.cmake" EXCLUDE)

# Patch upstream portfile.cmake to add --with-pic
file(READ "${VCPKG_ROOT_DIR}/ports/icu/portfile.cmake" upstream_content)
string(REPLACE "vcpkg_list(SET CONFIGURE_OPTIONS)" "vcpkg_list(SET CONFIGURE_OPTIONS)\nvcpkg_list(APPEND CONFIGURE_OPTIONS --with-pic)" modified_content "${upstream_content}")
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/portfile_upstream_patched.cmake" "${modified_content}")

# Include the patched portfile.cmake
include("${CMAKE_CURRENT_LIST_DIR}/portfile_upstream_patched.cmake")
