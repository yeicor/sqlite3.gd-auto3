
# CUSTOM: (hopefully all dependencies use cmake...)
set(VCPKG_CMAKE_CONFIGURE_OPTIONS ${VCPKG_CMAKE_CONFIGURE_OPTIONS}
    -DCMAKE_SHARED_LINKER_FLAGS=-Wl,-allow-multiple-definition  # Allow multiple definitions in the shared linker flags
)
