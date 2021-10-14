#!/bin/bash -e
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#create update image

function help() {
cat <<- EOF
	Create Update Image for the SOURCE_TARGET. This script should be run from build directory.

	${0} <SOURCE_TARGET> <VERSION> <PLATFORM>

	    SOURCE_TARGET    - Name of the target (usually PurePhone or BellHybrid)
	    VERSION          - version number to attach to file name
	    PLATFORM         - RT1051 or Linux

    In the current work dir, the script will create update image named from this template:
	<SOURCE_TARGET>-<VERSION>-<PLATFORM>-Update.tar



EOF
}

function setVars() {
    SOURCE_TARGET=${1}
    VERSION=${2}
    PLATFORM=${3}
    STAGING_DIR=update-package
    PACKAGE_FILE="${SOURCE_TARGET}-${VERSION}-${PLATFORM}-Update.tar"
    DEPS=(
        "sysroot/sys/current/${SOURCE_TARGET}-boot.bin"
        "version.json"
        "ecoboot.bin"
        "updater.bin"
        )
}

function checkForDeps() {
    local DEPS=${1}
    I=0
    DEPS_COUNT=${#DEPS[@]}
    while [[ I -lt ${DEPS_COUNT} ]]; do
        ITEM=${DEPS[${I}]}
        if [[ ! -e "${ITEM}" ]]; then
            echo "Couldn't find dependency: \"${ITEM}\""
            echo exti 2
        fi
        I=$(( I + 1))
    done
    echo "Deps-OK"
}

function linkInStageing(){
    pushd ${STAGING_DIR} 1> /dev/null
    ln -sf ../sysroot/sys/current/${SOURCE_TARGET}-boot.bin boot.bin
    ln -sf ../ecoboot.bin
    ln -sf ../updater.bin
    ln -sf ../${SOURCE_TARGET}-version.json version.json
    popd 1> /dev/null
}

function addChecksums() {
    pushd ${STAGING_DIR} 1> /dev/null
    rhash -u checksums.txt -r .
    popd 1> /dev/null
}

function compress() {
    tar chf ${PACKAGE_FILE} -C ${STAGING_DIR} .
}

if [[ $# -ne 3 ]]; then
    help
    exit 1
fi

setVars "${1}" "${2}" "${3}"
checkForDeps ${DEPS}
linkInStageing
addChecksums
compress


