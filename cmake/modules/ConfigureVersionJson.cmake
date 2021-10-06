list(APPEND CMAKE_MODULE_PATH "${SRC_DIR}/cmake/modules")
include(Version)

file(MD5 ${BOOTLOADER_FILE} BOOTLOADER_MD5SUM)
file(MD5 ${BOOT_FILE} BOOT_MD5SUM)
file(MD5 ${UPDATER_FILE} UPDATER_MD5SUM)

message("Configuring version.json file")
configure_file(${SRC_FILE} ${DST_FILE} @ONLY)
