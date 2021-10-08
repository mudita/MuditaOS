#!/bin/bash -e
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

source docker/.bashrc

function printVar(){
    echo "$1: '${!1}'"
}
echo "======================"
echo "       variables      "
printVar INPUT_ACTION
printVar INPUT_TARGET
printVar INPUT_BUILD_TYPE
printVar INPUT_BUILD_GENERATOR
printVar INPUT_JOBS
printVar PATH
printVar HOME
echo "======================"
printVar GITHUB_WORKSPACE
printVar GITHUB_RUN_ID
printVar GITHUB_ACTION
printVar GITHUB_ACTIONS
printVar GITHUB_ACTOR
printVar GITHUB_REPOSITORY
printVar GITHUB_EVENT_NAME
printVar GITHUB_SHA
printVar GITHUB_REF
printVar GITHUB_HEAD_REF
printVar GITHUB_BASE_REF
echo "======================"

GENERATOR=${INPUT_BUILD_GENERATOR,,}
CHANGELOG_FILE="changelog.md"

case ${GENERATOR} in
    "ninja")
        GENERATOR_NAME=${GENERATOR^}
        BUILD_CMD=${GENERATOR}
        ;;
    "make")
        GENERATOR_NAME=""
        BUILD_CMD=${GENERATOR}
        ;;
esac

if [[ "${INPUT_JOBS}" == "nproc" ]]; then
    JOBS=`nproc`
else 
    JOBS=${INPUT_JOBS}
fi

function styleCheck() {
    echo "style"
    ./config/pre-commit.hook --last
}

function configure() {
    echo "configure"
    ./configure.sh ${INPUT_TARGET} ${INPUT_BUILD_TYPE} ${GENERATOR_NAME:+-G ${GENERATOR_NAME}}
}

function build() {
    echo "build"
    pushd build-${INPUT_TARGET}-${INPUT_BUILD_TYPE} 
    ${BUILD_CMD} -j ${JOBS}
    popd
}

function package() {
    echo "package"
    pushd build-${INPUT_TARGET}-${INPUT_BUILD_TYPE} 
    ${BUILD_CMD} -j ${JOBS} package
    popd
}

function check() {
    echo "check"
    cd build-${INPUT_TARGET}-${INPUT_BUILD_TYPE} && ${BUILD_CMD} check -j ${JOBS}
}

function printChangeLogForGH() {
    CHANGELOG="$(cat ${CHANGELOG_FILE})"
    CHANGELOG="${CHANGELOG//'%'/'%25'}"
    CHANGELOG="${CHANGELOG//$'\n'/'%0A'}"
    CHANGELOG="${CHANGELOG//$'\r'/'%0D'}"
    echo "::set-output name=release_notes::${CHANGELOG}"
}

pushd ${GITHUB_WORKSPACE}
case ${INPUT_ACTION} in
    "style")
        styleCheck
        ;;
    "configure")
        configure
        ;;
    "build")
        build
        ;;
    "c&b")
        configure
        build
        ;;
    "check")
        configure
        check
        ;;
    "package")
        configure
        build
        package
        printChangeLogForGH
        ;;
    "*")
        echo "wrong action \"${INPUT_ACTION}\""
        exit 1
        ;;
esac

uptime 
echo "processor count: `nproc`"
