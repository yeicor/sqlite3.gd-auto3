include("${VCPKG_ROOT_DIR}/triplets/community/arm64-mingw-static.cmake")

set(VCPKG_CRT_LINKAGE static)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
set(VCPKG_CMAKE_CONFIGURE_OPTIONS "-DCMAKE_EXE_LINKER_FLAGS=-static-libgcc -static-libstdc++ -static")
