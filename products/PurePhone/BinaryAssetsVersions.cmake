# This file sets versions of downloaded binaries for release packaging purposes

if( NOT DEFINED ECOBOOT_BIN_VERSION)
    set(ECOBOOT_BIN_VERSION 1.0.17 CACHE STRING "bootloader binary version to download from bootloader release page")
endif()

if (NOT DEFINED UPDATER_BIN_VERSION)
    set(UPDATER_BIN_VERSION 1.1.2 CACHE STRING "updater binary version to download from updater release page")
endif()
