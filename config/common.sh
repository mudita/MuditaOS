#!/bin/bash -e
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

check_any_arch() {
    local path="${1}"
    local product="${2}"
    local arch="${3}"
    [ -d ${path} ] || ( echo "no such directory: ${path}" > /dev/stderr ; exit 1)
    file ${path}/${product}.elf | grep "$arch" -q || ( echo "Bad file: ${path}/${product}.elf for selected architecture!" ; exit 1 )
}

check_target_rt1051() {
    check_any_arch "${@}" 'ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), statically linked'
}
