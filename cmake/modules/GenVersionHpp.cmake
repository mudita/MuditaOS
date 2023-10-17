# This script generates the source/version.hpp containing project version
# information. It is meant to be run at build time by running CMake as a target.

list(APPEND CMAKE_MODULE_PATH "${SRC_DIR}/cmake/modules")
include(Version)
configure_file(
    ${SRC_DIR}/source/version.hpp.template
    ${CMAKE_BINARY_DIR}/source/include/product/version.hpp
    )
