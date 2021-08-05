#!/bin/bash -e
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

source config/common.sh

BIN_DIR="build-rt1051-Debug"
GDB_ARM=$( hash arm-none-eabi-gdb-py 2> /dev/null && echo "arm-none-eabi-gdb-py" || echo "arm-none-eabi-gdb" )

help() 
{
cat<<EOF
Script accepts two parameters: 
    - build* 
        build catalog, it will accept any catalog with "build" in name
    default: ${BIN_DIR} is taken

    --gdb PARAM
        parameter to full gdb path, for gcc9 you can pass i.e.
        ~/gcc-arm-none-eabi-8-2019-q3-update/bin/arm-none-eabi-gdb-py
    default: gdb from your PATH

examples:
./run.sh build-rt1051-RelWithDebInfo/ --gdb ~/gcc-arm-none-eabi-8-2019-q3-update/bin/arm-none-eabi-gdb-py   // full specification
./run.sh build-rt1051-RelWithDebInfo                                                                        // binary for gdb specified
./run.sh --gdb ~/gcc-arm-none-eabi-8-2019-q3-update/bin/arm-none-eabi-gdb-py                                // gdb binary specified
./run.sh                                                                                                    // full defaults
./run.sh --help                                                                                             // help
EOF
}

while [[ $# -gt 0 ]]; do
    echo "PARAM: $1"
    case $1 in
        build*)
            BIN_DIR="$1"
            ;;
        --gdb)
            GDB_ARM="$2"
            echo "Loaded $GDB_ARM"
            shift
            ;;
        --help)
            echo "HELP!"
            help
            exit 0
            ;;
        *)
            echo "Bad param! $1"
            help
            exit 1
            ;;
    esac;
    shift
done

check_target_rt1051 "${BIN_DIR}"

${GDB_ARM} "${BIN_DIR}"/BellHybrid.elf -x .gdbinit-1051
