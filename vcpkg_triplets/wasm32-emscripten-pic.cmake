include("triplets/community/wasm32-emscripten.cmake")

# CUSTOM: (hopefully all dependencies use cmake...)
set(VCPKG_CMAKE_CONFIGURE_OPTIONS ${VCPKG_CMAKE_CONFIGURE_OPTIONS}
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON  # Force POSITION_INDEPENDENT_CODE to ON
)

include("${CMAKE_CURRENT_LIST_DIR}/common/_allow-multiple-definition.cmake")
