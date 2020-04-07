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
function styleCheck() {
    echo "style"
    ./config/pre-commit-check-only.hook --last
}

function configure() {
    echo "configure"
    ./configure.sh ${INPUT_TARGET} ${INPUT_BUILD_TYPE}
}

function build() {
    echo "build"
    cd build-${INPUT_TARGET}-${INPUT_BUILD_TYPE} && make -j
}

function check() {
    echo "check"
    cd build-${INPUT_TARGET}-${INPUT_BUILD_TYPE} && make check -j
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

