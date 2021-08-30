#!/bin/bash -e
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#script is used on CI for creating daily builds

#this script should be run in docker container

LOGIN=${LOGIN}
TOKEN=${TOKEN}
IMAGE_NAME=${IMAGE_NAME:-wearemudita/mudita_os_builder:1.9}
WORK_DIR=${WORK_DIR:-MuditaOS}
REPOSITORY_URL=${REPOSITORY_URL:-https://${TOKEN}:x-oauth-basic@github.com/mudita/MuditaOS}
REPOSITORY_BRANCH=${REPOSITORY_BRANCH:-master}
ARTIFACTS_DIR=${ARTIFACTS_DIR:-/artifacts/${REPOSITORY_BRANCH}}
PRODUCT=${PRODUCT:-PurePhone}

CONFIG_LOG=config.log

VARS_TO_CHECK=(
        PRODUCT
        REPOSITORY_URL
        REPOSITORY_BRANCH
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

if [ ! -d ${ARTIFACTS_DIR} ]; then
    mkdir -p ${ARTIFACTS_DIR}
fi

if [ ! -d ${WORK_DIR} ]; then
    mkdir -p ${WORK_DIR}
fi

pushd ${WORK_DIR}

if checkIfGit; then
    git checkout master
    git pull --ff-only 
else
    echo "not a git dir"
    git clone -b ${REPOSITORY_BRANCH} ${REPOSITORY_URL} .
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
./configure.sh ${PRODUCT} rt1051 RelWithDebInfo -G Ninja
cd build-${PRODUCT,,}-rt1051-RelWithDebInfo
ninja
ninja ${PRODUCT}-StandaloneImage
ninja ${PRODUCT}-UpdatePackage

PKG_NAME_PREFIX=${PRODUCT}-${DATE}-RT1051
cp ${PKG_NAME_PREFIX}-image.tar.xz ${ARTIFACTS_DIR}
cp ${PKG_NAME_PREFIX}-Update.tar ${ARTIFACTS_DIR}

popd
