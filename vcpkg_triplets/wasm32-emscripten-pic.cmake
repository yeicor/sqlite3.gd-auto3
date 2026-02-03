include("triplets/community/wasm32-emscripten.cmake")

# CUSTOM: (hopefully all dependencies use cmake...)
set(VCPKG_CMAKE_CONFIGURE_OPTIONS ${VCPKG_CMAKE_CONFIGURE_OPTIONS}
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON  # Force POSITION_INDEPENDENT_CODE to ON
    -DCMAKE_SHARED_LINKER_FLAGS=-Wl,-allow-multiple-definition  # Allow multiple definitions in the shared linker flags
)