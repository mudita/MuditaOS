# This file sets versions of downloaded binaries for release packaging purposes

set(ECOBOOT_BIN_VERSION 1.1.0 CACHE STRING "bootloader binary version to download from bootloader release page")
set(UPDATER_BIN_VERSION 1.3.0 CACHE STRING "updater binary version to download from updater release page")
set(MUDITA_ASSETS_JSON ${CMAKE_CURRENT_SOURCE_DIR}/assets.json CACHE STRING "assets json description file to take to copy file to package build in curent directory")
set(MUDITA_ASSETS_DL_TYPE "github" CACHE STRING "assets download type from download_assets.py, mainly github and local")
