#!/bin/bash
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

cd ./build-linux-*
export LD_LIBRARY_PATH=$(realpath board/linux/libiosyscalls/libiosyscalls.so)
./PurePhone.elf

