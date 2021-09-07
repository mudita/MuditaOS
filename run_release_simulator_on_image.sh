#!/bin/bash -e
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md


pushd build-linux-Release

PRELOAD_LIB=$(realpath board/linux/libiosyscalls/libiosyscalls.so)
APP_BINARY="./PurePhone.elf"
RENDERER="./service_renderer"
if [ ! -x "$APP_BINARY" ]; then
    echo "Error: $APP_BINARY doesn't exists"
elif [ ! -x "$PRELOAD_LIB" ]; then
    echo "Error: $PRELOAD_LIB doesn't exists"
else
    echo "Running simulator in directory: $(pwd)"
    $RENDERER &
    export IOSYSCALLS_REDIRECT_TO_IMAGE=1
    export LD_PRELOAD=$PRELOAD_LIB
    exec "$APP_BINARY"
fi
