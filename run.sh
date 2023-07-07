#!/bin/bash -e
# Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

source config/common.sh

BIN_DIR="build-rt1051-Debug"

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

    --product PARAM
        product to run [PurePhone|BellHybrid]

examples:
./run.sh --product PurePhone  build-purephone-rt1051-RelWithDebInfo/ --gdb ~/gcc-arm-none-eabi-8-2019-q3-update/bin/arm-none-eabi-gdb-py // full specification
./run.sh --product BellHybrid build-bell-rt1051-RelWithDebInfo                                                                           // binary for gdb specified
./run.sh --product BellHybrid --gdb ~/gcc-arm-none-eabi-8-2019-q3-update/bin/arm-none-eabi-gdb-py                                        // gdb binary specified
./run.sh --help                                                                                                                          // help
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
        --product)
            PRODUCT_NAME="$2"
            echo "Product $PRODUCT_NAME"
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

if [ -z ${GDB_ARM+x} ]; then # if GDB_ARM unset, check which GDB binaries are present and choose one
    ALL_GDB_VERSIONS=( $(compgen -A command arm-none-eabi-gdb | sort -n) )
    PY_VER=()

    # check python version for all present GDB binaries
    for GDB_VERSION in "${ALL_GDB_VERSIONS[@]}"; do
        PY_VER+=( "$($GDB_VERSION --batch -iex 'python import sys; print(sys.version)' 2> /dev/null || echo "no python")" )
    done

    # check for GDB versions with python 3
    for i in "${!PY_VER[@]}"; do
        if [[ "${PY_VER[$i]}" == "3."* ]]; then 
            GDB_ARM=${ALL_GDB_VERSIONS[$i]}
            echo "GDB with python 3 found"
            break
        fi 
    done

    if [ -z ${GDB_ARM+x} ]; then # if GDB_ARM still unset check for GDB versions with python 2 
        for i in "${!PY_VER[@]}"; do
            if [[ "${PY_VER[$i]}" == "2."* ]]; then 
                GDB_ARM=${ALL_GDB_VERSIONS[$i]}
                echo "GDB with python 2 found. For the best debugging experience update your GDB binaries from the most recent toolchain release"
                break
            fi 
        done
    fi

    if [ -z ${GDB_ARM+x} ]; then 
        if command -v arm-none-eabi-gdb &> /dev/null; then
            echo "No GDB with python found. If you want to use debugging tools please install a GDB version that supports python."
            GDB_ARM="arm-none-eabi-gdb"
        else 
            echo "looks like you don't have any compatible GDB versions installed. Use the bootstrap.sh script to download and configure the ARM toolchain."
        fi
    fi
fi

if [ -z "${PRODUCT_NAME}" ]; then
    echo "ERROR! Product param not provided"
    help
    exit 1
fi

check_target_rt1051 "${BIN_DIR}" "${PRODUCT_NAME}"

${GDB_ARM} "${BIN_DIR}"/"${PRODUCT_NAME}".elf -x .gdbinit-1051
