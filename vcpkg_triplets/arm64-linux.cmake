include("triplets/community/arm64-linux.cmake")

# CUSTOM:
if(PORT STREQUAL "libpng")
    set(VCPKG_CMAKE_CONFIGURE_OPTIONS ${VCPKG_CMAKE_CONFIGURE_OPTIONS}
        -DPNG_ARM_NEON=off  # Disable PNG_ARM_NEON (avoids build error and supports more devices)
    )
endif()