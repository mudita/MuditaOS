#!/bin/bash -e
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# config file for pre-commit.hook clang-format
# ignore file for clang-format autoformatting

# set this variable in your shell if you wish to disable autoformatting on commit for time being
export DISABLE_AUTO_FORMATTING=0

# set this variable to get more verbose output
export VERBOSE=1

# ignore_paths for formatter - these are regex matched with filenames to be formatted
# if you don't wish to format i.e one file - just pass whole path to this file from git root directory
export declare ignore_paths=(
    '.*/catch.hpp'
    '.*/lib/'
    'build'
    'board/rt1051/xip/'
    'board/rt1051/gthr.h'
    'board/rt1051/newlib/cxx_guards.cpp'
    'board/rt1051/newlib/dir-common.h'
    'board/rt1051/newlib/include/'
    'board/rt1051/newlib/gcc11/'
    'board/rt1051/newlib/gcc12/'
    'host-tools/littlefs-fuse/lfsfuse/'
    'module-audio/Audio/decoder/dr_flac.h'
    'module-bluetooth/Bluetooth/btstack_config.h'
    'module-bluetooth/Bluetooth/glucode/bluetooth_init_cc2564C_1.0.c'
    'module-bluetooth/Bluetooth/glucode/btstack_uart_block_rt1051.h'
    'module-bluetooth/Bluetooth/glucode/hal_time_ms.c'
    'module-bluetooth/lib/btstack/'
    'module-bsp/board/rt1051/bellpx/bsp/audio/AW8898regs.hpp'
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
    'module-db/[Dd]atabase/shell.c'
    'module-db/[Dd]atabase/sqlite3.c'
    'module-db/[Dd]atabase/sqlite3.h'
    'module-db/[Dd]atabase/sqlite3ext.h'
    'module-lwip/includes/arch/cc.h'
    'module-lwip/includes/lwipopts.h'
    'module-lwip/lib/lwip/'
    'module-os/'
    'module-vfs/board/cross/freeRTOS_FAT/'
    'module-vfs/board/free_rtos_custom/include/FreeRTOSFATConfig.h'
    'module-vfs/drivers/include/thirdparty/fatfs/ffconf.h'
    'module-vfs/thirdparty/*'
    'third-party/'
)

# bash function using above config function
shouldnt_ignore() {
    # change full name path to path relative to root git dir
    local fname=${1/"$L_GIT_DIR"/"./"}
    for el in ${ignore_paths[@]}; do
        if [[ ${fname}  =~ ^${el}.* ]]; then
            [[ $VERBOSE ]] && echo "Ignore: ${fname} formatting due to: $el match!"
            return 1
        fi
    done
    return 0
}
