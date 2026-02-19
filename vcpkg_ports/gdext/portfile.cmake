# Portfile for sqlite3-gd: builds the Godot extension
# ABI change to force rebuild

# The dependencies are specified in vcpkg.json

set(SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

if(EXISTS "${SOURCE_PATH}/__GD_CMAKE_ARGS")
    file(READ "${SOURCE_PATH}/__GD_CMAKE_ARGS" GD_CMAKE_ARGS)
elseif(DEFINED ENV{GD_CMAKE_ARGS})
    set(GD_CMAKE_ARGS "$ENV{GD_CMAKE_ARGS}")
else()
    message(FATAL_ERROR "GD_CMAKE_ARGS environment variable OR ${SOURCE_PATH}/__GD_CMAKE_ARGS file not set.")
endif()
separate_arguments(GD_CMAKE_ARGS UNIX_COMMAND "${GD_CMAKE_ARGS}")
vcpkg_configure_cmake(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${GD_CMAKE_ARGS}
    MAYBE_UNUSED_VARIABLES GODOTCPP_PRECISION GODOTCPP_THREADS
)

vcpkg_build_cmake(TARGET install)

set(VCPKG_POLICY_EMPTY_INCLUDE_FOLDER enabled)
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
