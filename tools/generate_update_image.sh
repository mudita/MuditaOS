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
    STAGEING_DIR="${SOURCE_TARGET}-${VERSION}-${PLATFORM}-Update"
    PACKAGE_FILE="${STAGEING_DIR}.tar"
    DEPS=(
        "sysroot/sys/current/assets"
        "sysroot/sys/user"
        "sysroot/sys/current/${SOURCE_TARGET}-boot.bin"
        "sysroot/sys/current/country-codes.db"
        "sysroot/sys/current/Luts.bin"
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

function cleanStagingDir(){
    local STAGEING_DIR=${1}
    if [[ -d ${STAGEING_DIR} ]]; then
        rm -Rf ${STAGEING_DIR}
    fi
    mkdir ${STAGEING_DIR} -v
}

function linkInStageing(){
    pushd ${STAGEING_DIR} 1> /dev/null

    mkdir assets
    pushd assets 1> /dev/null
    ln -s ../../sysroot/sys/current/assets/fonts
    ln -s ../../sysroot/sys/current/assets/images
    ln -s ../../sysroot/sys/current/assets/lang
    popd 1> /dev/null

    ln -s ../sysroot/sys/user
    ln -s ../sysroot/sys/current/${SOURCE_TARGET}-boot.bin boot.bin
    ln -s ../sysroot/sys/current/country-codes.db
    ln -s ../sysroot/sys/current/Luts.bin
    ln -s ../ecoboot.bin
    ln -s ../updater.bin
    ln -s ../${SOURCE_TARGET}-version.json version.json
    popd 1> /dev/null
}

function addChecksums() {
    pushd ${STAGEING_DIR} 1> /dev/null
    rhash -u checksums.txt -r .
    popd 1> /dev/null
}

function compress() {
    tar chf ${PACKAGE_FILE} -C ${STAGEING_DIR} .
}

if [[ $# -ne 3 ]]; then
    help
    exit 1
fi

setVars "${1}" "${2}" "${3}"
checkForDeps ${DEPS}
cleanStagingDir ${STAGEING_DIR}
linkInStageing
addChecksums
compress


