#!/usr/bin/env bash
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

function if_cmd_installed() {
    if ! command -v "$1"  &> /dev/null; then
        echo "$1 command could not be found. Please install it using system package manager"
        exit 1
    fi
}

if_cmd_installed "luarocks"

luarocks install lsqlite3complete 
luarocks install lfs
luarocks install luaunit