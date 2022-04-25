#!/bin/bash -e
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

arraymenu ()
{
  echo "Number of build images: $#"
  echo "$@"
  select option; do # in "$@" is the default
    if [ "$REPLY" -eq "$#" ];
    then
      echo "Exiting..."
      break;
    elif [ 1 -le "$REPLY" ] && [ "$REPLY" -le $(($#-1)) ];
    then
      echo "You selected $option simulator image."
      break;
    else
      echo "Incorrect Input: Select a number 1-$#"
    fi
  done
}

readarray -t dirs < <(find . -maxdepth 1 -type d -iname 'build-linux-*' -printf '%P\n')

if [ ${#dirs[@]} -eq 1 ]; then
    cd "${dirs[0]}"
elif [ ${#dirs[@]} -gt 1 ]; then
    arraymenu "${dirs[@]}"
    cd "${dirs[$REPLY-1]}"
else
    echo "Error: Simulator directory doesn't exists"
    exit 1
fi

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
