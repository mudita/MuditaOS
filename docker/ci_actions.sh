#!/bin/bash -e

source /home/docker/.bashrc

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
    cd build-${INPUT_TARGET}-${INPUT_BUILD_TYPE} && ${BUILD_CMD} -j ${JOBS}
}

function check() {
    echo "check"
    cd build-${INPUT_TARGET}-${INPUT_BUILD_TYPE} && ${BUILD_CMD} check -j ${JOBS}
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
    "*")
        echo "wrong action \"${INPUT_ACTION}\""
        exit 1
        ;;
esac

uptime 
echo "procesor count: `nproc`"
