#!/bin/bash -e
[[ -e $1 ]] || ( echo "ERROR: Provide proper folder with PurePhone.elf to load"; exit 1)
arm-none-eabi-gdb $1/PurePhone.elf -x .gdbinit-1051
