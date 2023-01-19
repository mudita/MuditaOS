#!/usr/bin/env bash
# Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

function if_cmd_installed() {
    if ! command -v "$1"  &> /dev/null; then
        echo "$1 command could not be found. Please install it using system package manager"
        exit 1
    fi
}

function install_ltar(){
wget https://github.com/mudita/ltar/archive/master.zip -P temp
cd temp
tar -xvf master.zip && cd ltar-master
luarocks make 
cd ../..
rm -r temp
}

function copy_assets(){
    mkdir -p assets
    cp -r ../products/PurePhone/assets/ assets/
}

if_cmd_installed "luarocks"

luarocks install lunajson 
luarocks install lfs
luarocks install busted

install_ltar
copy_assets
