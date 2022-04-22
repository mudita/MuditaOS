#!/bin/bash
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set -euo pipefail
#set -o posix

test_for_sudo () {
  if eval "sudo -n echo >> /dev/null 2>&1"; then
    export can_sudo=1
  else
    export can_sudo=0
  fi
}

get_command () {
  if command -v "$name" >/dev/null 2>&1; then
    return 0
  fi
  if [ ! $can_sudo ]; then
    return 1
  fi
  if ! path_val=$(sudo -nl "$1" 2>/dev/null); then
    return 2
  elif [ ! -f "$path_val" ]; then
    return 1
  fi
  return 0
}

require_unprivileged () {
  for name in "$@"; do
    if ! command -v "$name" >/dev/null 2>&1; then
      if [ $can_sudo ] && [ "$(sudo -nl "$name" 2>/dev/null)" ]; then
        echo "Error: user level permissions are required for $name. Unable to proceed."
      else
        echo "Error: $name is not installed. Unable to proceed."
      fi
      exit 1
    fi
  done
}

require_commands () {
  for name in "$@"; do
    case $(command -v "$name" >/dev/null 2>&1) in
      1)
        if [ $can_sudo ]; then
          echo "Error: $name is not installed. Unable to proceed."
        else
          echo "Error: $name is not installed OR you do not have sufficent permissions. Unable to proceed."
        fi
        exit 1
      ;;
      2)
        echo "Error: You do not have sufficient permissions to execute $name. Unable to proceed."
        exit 2
      ;;
      0)
        echo "Success."
      ;;
    esac
  done
}

maysudo () {
  if [ "$(command -v "$1" >/dev/null 2>&1)" ] &&
     [ -f "$(command -v "$1")" ]; then
    "$@"
  elif [ ! $can_sudo ] ||
       [ ! -f "$(sudo -nl "$1" 2>/dev/null)" ]; then
    exit 1
  else
    sudo -n "$@"
  fi
}

test_for_sudo
