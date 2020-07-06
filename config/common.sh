#!/bin/bash -e
PURE_PARTITION_PRIMARY="PUREOS"
PURE_PARTITION_RECOVERY="RECOVER"
PURE_OS_CURRENT="current"
PURE_OS_PREVIOUS="previous"
PURE_OS_FACTORY="factory"

IMAGE_FILES="assets country-codes.db Luts.bin user .boot.ini .boot.ini.crc32"
IMAGE_MOVE_TO_ROOT="user .boot.ini .boot.ini.crc32"
check_any_arch() {
    local path="${1}"
    local arch="${2}"
    [ -d ${path} ] || ( echo "no such directory: ${path}" > /dev/stderr ; exit 1)
    file ${path}/PurePhone.elf | grep "$arch" -q || ( echo "Bad file: $1/PurePhone.elf for selected architecture!" ; exit 1 )
}

check_target_rt1051() {
    check_any_arch "${@}" 'ELF 32-bit LSB executable, ARM, EABI5 version 1 (SYSV), statically linked'
}

check_target_linux() {
    check_any_arch "${@}" '(GNU/Linux)'
}
