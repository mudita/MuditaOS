#!/bin/bash -e
PURE_PARTITION_PRIMARY="PUREOS"
PURE_PARTITION_RECOVERY="RECOVER"
PURE_OS_CURRENT="current"
PURE_OS_PREVIOUS="previous"
PURE_OS_FACTORY="factory"
PURE_OS_VERSION_MAJOR=""
PURE_OS_VERSION_MINOR=""
PURE_OS_VERSION_PATCH=""
PURE_OS_GIT_TAG=""
PURE_US_GIT_REV=""
PURE_OS_GIT_BRANCH=""

IMAGE_FILES="assets country-codes.db Luts.bin user .boot.json .boot.json.crc32"
IMAGE_MOVE_TO_ROOT="user .boot.json .boot.json.crc32"
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

PURE_OS_GIT_TAG=`git describe --tags | awk -F'-' '{print $2}'`
PURE_OS_GIT_REV=`git log --pretty=format:'%h' -n 1`
PURE_OS_GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`

version=( ${PURE_OS_GIT_TAG//./ } )
PURE_OS_VERSION_MAJOR=${version[0]}
PURE_OS_VERSION_MINOR=${version[1]}
PURE_OS_VERSION_PATCH=${version[2]}
