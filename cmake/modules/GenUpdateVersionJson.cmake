# This script generates the version.json file which contains project
# and bootloader version information for update packages . It is meant to be run
# at build time by running CMake as a target.

list(APPEND CMAKE_MODULE_PATH "${SRC_DIR}/cmake/modules")
include(Version)

set(BOOTLOADER_INCLUDED "true")
set(BOOTLOADER_FILENAME "ecoboot.bin")
execute_process(
    COMMAND grep "release:" "${ECOBOOT_DOWNLOAD_LOG}"
    COMMAND awk "{print $2}"
    OUTPUT_VARIABLE BOOTLOADER_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(
    ${SRC_DIR}/config/version.json.cmake_template
    ${CMAKE_BINARY_DIR}/version.json
    @ONLY
    )
