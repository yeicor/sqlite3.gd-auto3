include("${VCPKG_ROOT_DIR}/triplets/community/wasm32-emscripten.cmake")

# Enable PIC for Emscripten
set(VCPKG_CMAKE_CONFIGURE_OPTIONS "${VCPKG_CMAKE_CONFIGURE_OPTIONS} -DCMAKE_POSITION_INDEPENDENT_CODE=ON")
