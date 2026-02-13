# No-op portfile for sqlite3-gd meta-port
# This port ensures sqlite3 is installed with the correct configuration

# The dependencies are specified in vcpkg.json

set(VCPKG_POLICY_EMPTY_INCLUDE_FOLDER enabled)
vcpkg_install_copyright(FILE_LIST "${CMAKE_CURRENT_LIST_DIR}/../../LICENSE")
