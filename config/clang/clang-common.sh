#!/bin/env bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set -eo pipefail

verify_clang_format_version()
{
    # check if clang-format in path is in proper version, version is 3rd column in `clang-format --version`
    local clang_format
    clang_format=$( find  /usr/bin/ -regextype egrep -regex ".*/clang-format(-[0-9][0-9])?")
    local version
    version=$( [[ $(which $clang_format) ]] && ($clang_format --version | sed "s/.*version \([^ ]*\).*$/\1/" |  cut -d'.' -f1) || echo "0")
    echo $version
    # check for either clang-format or clang-format-11
    if [[ $version -lt 11 ]]; then
        cat << EOF >&1
Either install:
    clang-format in at least version 11 and set as default"
    or
    clang-format-11

    Your clang format version in path used:
        $(clang-format --version):
        $(clang-format-11 --version)
    git commit aborted"
EOF
        exit 1
    fi
}

## find `what` from list of directories `arr`
get_tool() {
    local what="$1"; shift
    local arr=("$@")

    local tool="";
    for el in ${arr[*]}; do
        if [[ -f "${el}" ]]; then
            tool=${el}
            break
        fi
    done
    if [[ ${tool} == "" ]]; then
        echo "$what not found in path and: ${arr[*]}" > /dev/stderr
        exit 2
    fi
    echo "${tool}"
}

## Search for clang-format-diff.py
get_clang_format() {
    local searchpaths=(
        "$(which "clang-format-diff.py" 2>/dev/null 1>/dev/null)"  # clang-format-diff in path
        "/usr/share/clang/clang-format-*/clang-format-diff.py"     # clang-format-diff location on Ubuntu/Debian
        "/usr/share/clang/clang-format-diff.py"                    # clang-format_diff location on Arch last resort
    )
    get_tool "clang-format-diff.py" "${searchpaths[@]}"
}

## search for clang-tidy-diff
get_clang_tidy()
{
    local searchpaths=(
        "$(which "clang-tidy-diff.py" 2>/dev/null 1>/dev/null)"  # clang-format-diff in path
        "/usr/bin/clang-tidy-diff-10.py"                         # clang-format-diff location on Ubuntu
        "/usr/bin/clang-tidy-diff-12.py"
        "/usr/share/clang/clang-tidy-*/clang-tidy-diff.py"       # clang-format-diff location on Debian
        "/usr/share/clang/clang-tidy-diff.py"                    # clang-format_diff location on Arch last resort
    )
    get_tool "clang-tidy-diff.py" "${searchpaths[@]}"
}
