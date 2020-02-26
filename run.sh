#!/bin/bash -e
[[ -e $1 ]] || ( echo "ERROR: Provide proper folder with PurePhone.elf to load"; exit 1)
make -C $1 -j 4 && \
~/gcc-arm-none-eabi-8-2019-q3-update/bin/arm-none-eabi-gdb-py $1/PurePhone.elf -x .gdbinit-1051
