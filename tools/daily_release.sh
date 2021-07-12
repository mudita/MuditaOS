#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#script is used on CI for creating daily builds

#this script should be run in docker container

set -e

LOGIN=${LOGIN}
TOKEN=${TOKEN}
IMAGE_NAME=${IMAGE_NAME:-wearemudita/mudita_os_builder:1.9}
WORK_DIR=${WORK_DIR:-MuditaOS}
ARTEFACTS_DIR=${ARTEFACTS_DIR:-/artefacts}
REPOSITORY_URL=${REPOSITORY_URL:-https://${TOKEN}:x-oauth-basic@github.com/mudita/MuditaOS}

CONFIG_LOG=config.log

VARS_TO_CHECK=(
        REPOSITORY_URL
        LOGIN
        TOKEN
        IMAGE_NAME
    )

function printVar(){
    echo "$1: '${!1}'"
}

function checkVariables() {
    I=0
    while [[ $I -lt ${#VARS_TO_CHECK[@]} ]]
    do
        printVar ${VARS_TO_CHECK[$I]}
        if [[ -z "${!VARS_TO_CHECK[$I]}" ]]; then
            echo "${VARS_TO_CHECK[$I]} is not set - aborting" 1>&2
            exit 1
        fi
        I=$(( I + 1 ))
    done
}

function checkIfGit() {
    if [[ -d .git ]]; then
        echo .git;
    else 
        git rev-parse --git-dir 2> /dev/null;
    fi;
}

function addTokens(){
    git config user.githublogin ${LOGIN}
    git config user.apitoken ${TOKEN}
}


##################### script ####################
checkVariables

if [ ! -d ${WORK_DIR} ]; then
    mkdir ${WORK_DIR}
fi

pushd ${WORK_DIR}

if checkIfGit; then
    git checkout master
    git pull --ff-only 
else
    echo "not a git dir"
    git clone ${REPOSITORY_URL} .
    git checkout master
fi

if [ ! -d ccache ]; then
    mkdir ccache
fi

CCACHE_DIR=$(pwd)/ccache
export CCACHE_DIR


DATE=$(date "+%Y.%m.%d")

addTokens
git submodule update --init --recursive
git tag -f daily-${DATE}
git push --tags origin
./configure.sh rt1051 RelWithDebInfo -G Ninja &> ${CONFIG_LOG}
cat ${CONFIG_LOG}
PKG_NAME_PREFIX=$(cat ${CONFIG_LOG} | grep CPACK_PACKAGE_FILE_NAME  | cut -f2 -d: | tr -d "'")
cd build-rt1051-RelWithDebInfo
ninja
ninja package-standalone
ninja package-update

cp ${PKG_NAME_PREFIX}-image.tar.xz ${ARTEFACTS_DIR}
cp ${PKG_NAME_PREFIX}-Update.tar ${ARTEFACTS_DIR}

popd



