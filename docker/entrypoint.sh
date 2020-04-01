#!/bin/bash

source /home/docker/.bashrc

function printVar(){
    echo "$1: '${!1}'"
}

printVar GITHUB_WORKSPACE
printVar INPUT_ACTION
printVar INPUT_TARGET
printVar INPUT_BUILD_TYPE
printVar PATH
arm-none-eabi-g++ --version
cmake --version
