# Portfile for sqlite3-gd: builds the Godot extension
# ABI change to force rebuild

# The dependencies are specified in vcpkg.json

set(SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

if(EXISTS "${SOURCE_PATH}/__GD_CMAKE_ARGS")
    file(READ "${SOURCE_PATH}/__GD_CMAKE_ARGS" GD_CMAKE_ARGS)
else()
    set(GD_CMAKE_ARGS "-DGODOTCPP_TARGET=template_debug -DGODOTCPP_PRECISION=single -DBUILD_TESTING=ON")
endif()
separate_arguments(GD_CMAKE_ARGS UNIX_COMMAND "${GD_CMAKE_ARGS}")

vcpkg_configure_cmake(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${GD_CMAKE_ARGS}
    MAYBE_UNUSED_VARIABLES GODOTCPP_PRECISION
)

vcpkg_build_cmake(TARGET sqlite3_gd_tests)

vcpkg_build_cmake(TARGET install)

vcpkg_execute_required_process(
    COMMAND ctest --output-on-failure
    WORKING_DIRECTORY "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-dbg"
    LOGNAME test-dbg
)

vcpkg_execute_required_process(
    COMMAND ctest --output-on-failure
    WORKING_DIRECTORY "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel"
    LOGNAME test-rel
)

set(VCPKG_POLICY_EMPTY_INCLUDE_FOLDER enabled)
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
