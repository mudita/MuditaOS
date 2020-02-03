#!/bin/bash
make -C build -j 4 && \
~/gcc-arm-none-eabi-8-2019-q3-update/bin/arm-none-eabi-gdb-py build/PurePhone.elf -x .gdbinit-1051
