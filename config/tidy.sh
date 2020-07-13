#!/usr/bin/env bash -e
# script to call clang tidy on whatever platform

L_GIT_DIR=$(git rev-parse --show-toplevel)
source $L_GIT_DIR/config/format-config.sh
source $L_GIT_DIR/config/find-tool.sh

clang_tools_assert

git diff origin/master | $get_clang_tidy_script -p 1
