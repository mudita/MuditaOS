#!/bin/bash -e
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# config file for pre-commit.hook clang-format
# ignore file for clang-format autoformating

# set this variable in your shell if you wish to disable autoformatting on commit for time being
#DISABLE_AUTO_FORMATTING=1

# set this variable to get more verbose output
VERBOSE=1

# ignore_paths for formatter - these are regex matched with filenames to be formated
# if you don't wish to format i.e one file - just pass whole path to this file from git root directory
declare ignore_paths=(
    '.*/catch.hpp'
    '.*/lib/'
    'board/'
    'build'
    'host-tools/littlefs-fuse/lfsfuse/'
    'module-audio/Audio/decoder/dr_flac.h'
    'module-bluetooth/Bluetooth/btstack_config.h'
    'module-bluetooth/Bluetooth/glucode/bluetooth_init_cc2564C_1.0.c'
    'module-bluetooth/Bluetooth/glucode/btstack_uart_block_rt1051.h'
    'module-bluetooth/Bluetooth/glucode/hal_time_ms.c'
    'module-bluetooth/lib/btstack/'
    'module-bsp/board/rt1051/bsp/eMMC/'
    'module-bsp/board/rt1051/bsp/pwr/'
    'module-bsp/board/rt1051/common/board.cpp'
    'module-bsp/board/rt1051/common/board.h'
    'module-bsp/board/rt1051/common/clock_config.h'
    'module-bsp/board/rt1051/common/cmsis/'
    'module-bsp/board/rt1051/common/fsl_drivers/'
    'module-bsp/board/rt1051/common/pin_mux.h'
    'module-bsp/board/rt1051/common/pin_mux.c'
    'module-bsp/board/rt1051/common/system_MIMXRT1051.c'
    'module-bsp/board/rt1051/common/system_MIMXRT1051.h'
    'module-bsp/bsp/'
    'module-db/[Dd]atabase/config.h'
    'module-db/[Dd]atabase/sqlite3.c'
    'module-db/[Dd]atabase/sqlite3.h'
    'module-db/[Dd]atabase/sqlite3ext.h'
    'module-lwip/includes/arch/cc.h'
    'module-lwip/includes/lwipopts.h'
    'module-lwip/lib/lwip/'
    'module-os/'
    'module-utils/json/'
    'module-utils/microtar/'
    'module-utils/segger/log/'
    'module-utils/segger/systemview/'
    'module-utils/tinyfsm/'
    'module-vfs/board/cross/freeRTOS_FAT/'
    'module-vfs/board/free_rtos_custom/include/FreeRTOSFATConfig.h'
    'module-vfs/drivers/include/thirdparty/fatfs/ffconf.h'
    'module-vfs/thirdparty/*'
)
