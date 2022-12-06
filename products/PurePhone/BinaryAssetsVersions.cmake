# This file sets versions of downloaded binaries for release packaging purposes

if( NOT DEFINED ECOBOOT_BIN_VERSION)
    set(ECOBOOT_BIN_VERSION 2.0.1 CACHE STRING "bootloader binary version to download from bootloader release page")
endif()

if (NOT DEFINED RECOVERY_BIN_VERSION)
    set(RECOVERY_BIN_VERSION 1.0.1 CACHE STRING "recovery binary version to download from recovery release page")
endif()
