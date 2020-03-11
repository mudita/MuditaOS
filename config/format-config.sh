#!/bin/bash -e
# config file for pre-commit.hook clang-format
# ignore file for clang-format autoformating

# set this variable in your shell if you wish to disable autoformatting on commit for time being
# DISABLE_AUTO_FORMATTING=1

# set this variable to get more verbose output
VERBOSE=1

# ignore_paths for formatter - these are regex matched with filenames to be formated
# if you don't wish to format i.e one file - just pass whole path to this file from git root directory
declare ignore_paths=(
    './module-bsp/bsp/'
    './module-vfs/board/cross/freeRTOS_FAT'
    './module-db/Database/sqlite3.h'
    './module-db/Database/sqlite3.c'
    './module-db/Database/config.h'
    './module-bluetooth/lib/btstack'
    './module-lwip/lib/lwip'
    './module-utils/tinyfsm'
    './module-utils/microtar'
)
