#!/bin/bash

if [[ $BASH_VERSINFO -lt 4 ]]; then
    echo "Please update your bash to at last version 4"
	echo "your is: ${BASH_VERSINFO}"
    exit 4
fi

function help() {
    echo -e "Use this script for faster configuring build types"
    echo -e "ussage:"
    echo -e "\t$0 <target> <build_type> [systemview]"
    echo -e "available targets are:"
    echo -e "\t\t\tlinux\n\t\t\trt1051"
    echo -e "available build types:"
    echo -e "\t\t\tDebug\t\t- standard debug build"
    echo -e "\t\t\tRelease\t\t- release build (not for debugging)"
    echo -e "\t\t\tRelWithDebInfo\t - release with debug info in separate file"
    echo -e "available systemview options:"
    echo -e "\t\t\tSYSTEMVIEW_OFF\t\t- default - disabled"
    echo -e "\t\t\tSYSTEMVIEW_ON\t\t- enabled"
    echo -e "\n\e[1m\e[31mThis script will delete previous build dir!\e[0m"
}

function test_env.cmake() {
    ENV_FILE="env.cmake"
    if [ ! -f ${ENV_FILE} ]; then
        echo "there's no ${ENV_FILE} please check README.md on how to create one"
        exit 2
    fi
}

function check_target() {
    case ${TARGET,,} in
        linux ) 
            CMAKE_TOOLCHAIN_FILE="Target_Linux.cmake"
            return 0 ;;
        rt1051 ) 
            CMAKE_TOOLCHAIN_FILE="Target_RT1051.cmake"
            return 0 ;;
        *) 
            echo "Wrong target: \"${TARGET}\""
            return 1
            ;;
    esac
}
function check_build_type() {
    case ${BUILD_TYPE,,} in
        debug)
            CMAKE_BUILD_TYPE="Debug"
            return 0;;
        release)
            CMAKE_BUILD_TYPE="Release"
            return 0;;
        relwithdebinfo)
            CMAKE_BUILD_TYPE="RelWithDebInfo"
            return 0;;
        *)
            echo "wrong build type \"${BUILD_TYPE}\""
            return 1;;
    esac
}

function check_systemview() {
    case ${SYSTEMVIEW,,} in
        systemview_off)
            SYSTEMVIEW="OFF"
            return 0;;
        systemview_on)
            SYSTEMVIEW="ON"
            return 0;;
        *)
            echo "wrong systemview option \"${SYSTEMVIEW}\" using default OFF"
            SYSTEMVIEW="OFF"
            return 0;;
    esac
}

if [[ ($# -lt 2) || ($# -gt 3)]]; then
    help
    exit 1
fi
test_env.cmake

TARGET=$1
BUILD_TYPE=$2
SYSTEMVIEW=$3

if check_target && check_build_type ; then
    check_systemview
    BUILD_DIR="build-${TARGET,,}-${CMAKE_BUILD_TYPE}"
    echo -e "build dir:\e[34m\n\t${BUILD_DIR}\e[0m"
    SRC_DIR=`pwd`
    if [ -d ${BUILD_DIR} ]; then
        rm -Rf ${BUILD_DIR}
    fi
    if [ -e ${BUILD_DIR} ]; then
        echo "couldn't delete: ${BUILD_DIR}"
        exit 6
    fi
    mkdir -p ${BUILD_DIR}
    if cd ${BUILD_DIR} ; then
        CMAKE_CMD="cmake \
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
                    -DCMAKE_TOOLCHAIN_FILE=${SRC_DIR}/${CMAKE_TOOLCHAIN_FILE} \
                    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ${SRC_DIR} \
                    -DSYSTEMVIEW=${SYSTEMVIEW} "
        echo -e "\e[32m${CMAKE_CMD}\e[0m" | tr -s " "
        if $CMAKE_CMD; then
            echo -e "\e[32mcd ${BUILD_DIR} && make -j\e[0m"
        else
            echo -e "configuration error!"
        fi
    else
        echo "Cannot switch to\"$BUILD_DIR\"!"
        exit 5
    fi
else
    echo "stop"
    exit 3
fi



