#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

if [[ $BASH_VERSINFO -lt 4 ]]; then
    echo "Please update your bash to at last version 4"
    echo "your is: ${BASH_VERSINFO}"
    exit 4
fi

function help() {
    echo -e "Use this script for faster configuring build types"
    echo -e "ussage:"
    echo -e "\t$0 [-dh] <product> <target> <build_type> [other cmake options]"
    echo -e "options:"
    echo -e "\t\t\td\t\t- turn on development features"
    echo -e "\t\t\th\t\t- print this help"
    echo -e "available products are:"
    echo -e "\t\t\tpure"
    echo -e "\t\t\tbell"
    echo -e "available targets are:"
    echo -e "\t\t\tlinux\n\t\t\trt1051"
    echo -e "available build types:"
    echo -e "\t\t\tDebug\t\t- standard debug build"
    echo -e "\t\t\tRelease\t\t- release build (not for debugging)"
    echo -e "\t\t\tRelWithDebInfo\t- release with debug info in separate file"
    echo -e "\n\e[1m\e[31mThis script will delete previous build dir!\e[0m"
}

function validate_product_selection() {
    case ${PRODUCT,,} in
        pure | purephone)
            PRODUCT_SHORTNAME="pure"
            PRODUCT="PurePhone"
            return 0 ;;
        bell | bellhybrid)
            PRODUCT_SHORTNAME="bell"
            PRODUCT="BellHybrid"
            return 0 ;;
        *)
            echo "Wrong product: \"${PRODUCT}\""
    esac
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

function github_return_build_dir() {
    if [[ -n "${GITHUB_WORKSPACE}" ]]; then
        echo "setting out package_path:$1"
        echo "::set-output name=package_path::$1"
    fi
}

DEVELOPMENT_FEATURES="OFF"
while getopts ":d" o; do
    case "${o}" in
    d)
        DEVELOPMENT_FEATURES="ON"
        ;;
    h)
        help
        exit 0
	;;
    *)
        help
        exit 1
        ;;
    esac
done
shift $((OPTIND-1))

PRODUCT=$1
TARGET=$2
BUILD_TYPE=$3

if validate_product_selection && check_target && check_build_type ; then
    shift 3
    BUILD_DIR="build-${PRODUCT}-${TARGET,,}-${CMAKE_BUILD_TYPE}"
    echo -e "build dir:\e[34m\n\t${BUILD_DIR}\e[0m"
    SRC_DIR=`pwd`
    if [ -d ${BUILD_DIR} ]; then
        rm -Rf ${BUILD_DIR}
    fi
    if [ -e ${BUILD_DIR} ]; then
        echo "couldn't delete: ${BUILD_DIR}"
        exit 6
    fi
    github_return_build_dir ${BUILD_DIR}
    mkdir -p ${BUILD_DIR}
    if cd ${BUILD_DIR} ; then
        CMAKE_CMD="cmake \
                    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
                    -DCMAKE_TOOLCHAIN_FILE=${SRC_DIR}/${CMAKE_TOOLCHAIN_FILE} \
                    -DPRODUCT=${PRODUCT} \
                    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
                    -DWITH_DEVELOPMENT_FEATURES=${DEVELOPMENT_FEATURES} \
                    $@ \
                    ${SRC_DIR} "
        echo -e "\e[32m${CMAKE_CMD}\e[0m" | tr -s " "
        if $CMAKE_CMD; then
            Ninja=$(echo $@ | grep "Ninja")
            if [[ -z ${Ninja} ]]; then
                echo -e "\e[32mcd ${BUILD_DIR} && make -j $(nproc) <Pure|Bell>\e[0m"
            else
                echo -e "\e[32mcd ${BUILD_DIR} && ninja <Pure|Bell>\e[0m"
            fi
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



