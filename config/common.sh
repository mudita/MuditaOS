#!/bin/bash -e
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

MUDITAOS_PARTITION_PRIMARY="MUDITAOS"
MUDITAOS_PARTITION_RECOVERY="RECOVER"
MUDITAOS_CURRENT="current"
MUDITAOS_PREVIOUS="previous"
MUDITAOS_FACTORY="factory"
MUDITAOS_UPDATES="updates"
MUDITAOS_VERSION_MAJOR=""
MUDITAOS_VERSION_MINOR=""
MUDITAOS_VERSION_PATCH=""
MUDITAOS_GIT_TAG=""
MUDITAUS_GIT_REV=""
MUDITAOS_GIT_BRANCH=""

IMAGE_FILES="data assets country-codes.db Luts.bin user .boot.json .boot.json.crc32"
IMAGE_MOVE_TO_ROOT="user .boot.json .boot.json.crc32"
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

MUDITAOS_GIT_TAG=`git describe --tags | awk -F'-' '{print $2}'`
MUDITAOS_GIT_REV=`git log --pretty=format:'%h' -n 1`
MUDITAOS_GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`

version=( ${MUDITAOS_GIT_TAG//./ } )
MUDITAOS_VERSION_STRING=$MUDITAOS_GIT_TAG
MUDITAOS_VERSION_MAJOR=${version[0]}
MUDITAOS_VERSION_MINOR=${version[1]}
MUDITAOS_VERSION_PATCH=${version[2]}
