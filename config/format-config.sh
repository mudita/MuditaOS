#!/bin/bash -e
# config file for pre-commit.hook clang-format
# ignore file for clang-format autoformating

# set this variable in your shell if you wish to disable autoformatting on commit for time being
#DISABLE_AUTO_FORMATTING=1

# set this variable to get more verbose output
VERBOSE=1

# ignore_paths for formatter - these are regex matched with filenames to be formated
# if you don't wish to format i.e one file - just pass whole path to this file from git root directory
declare ignore_paths=(
    './module-os/'
    './board/'
    './module-bsp/bsp/'
    './module-bsp/board/rt1051/common/fsl_drivers/'
    './module-bsp/board/rt1051/common/board.cpp'
    './module-vfs/board/cross/freeRTOS_FAT/'
    './module-db/[Dd]atabase/sqlite3.h'
    './module-db/[Dd]atabase/sqlite3.c'
    './module-db/[Dd]atabase/config.h'
    './module-bluetooth/lib/btstack/'
    './module-lwip/lib/lwip/'
    './module-utils/tinyfsm/'
    './module-utils/microtar/'
    './module-utils/json/'
    './module-utils/segger/log/'
    './module-utils/segger/systemview/'
    './build'
    './.*/lib/'
    './.*/catch.hpp'

)
