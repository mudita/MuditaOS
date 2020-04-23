#!/bin/bash -e

source config/common.sh

BIN_DIR="build-rt1051-Debug"
GDB_ARM=$( hash arm-none-eabi-gdb-py 2> /dev/null && echo "arm-none-eabi-gdb-py" || echo "arm-none-eabi-gdb" )

help() 
{
cat<<EOF
Script accept one parameter: build catalog
- It will accept any catalog with "build" in name
- default ${BIN_DIR} is taken

It will try tu use "arm-none-eabi-gdb-py" if exists
in path else it will use "arm-none-eabi-gdb"
EOF
}

while [[ $# -gt 0 ]]; do
    case $1 in
        build*)
            BIN_DIR="$1"
            ;;
        *)
            help
            exit 0
            ;;
    esac;
    shift
done

check_target_rt1051 "${BIN_DIR}"

${GDB_ARM} "${BIN_DIR}"/PurePhone.elf -x .gdbinit-1051
