#!/bin/bash
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set -euo pipefail

# shellcheck source=../config/format-config.sh
# shellcheck source=../config/clang/colors.sh
# shellcheck source=../config/clang/common.sh

L_GIT_DIR=$(git rev-parse --show-toplevel)
source $L_GIT_DIR/config/format-config.sh
source $L_GIT_DIR/config/clang/clang-common.sh
source $L_GIT_DIR/config/clang/colors.sh

help()
{
    cat <<- EOF
clang tidy diff tool used to perform check on our sources uses 2 defaults:
    .clang-tidy                 : configuration file, from the place where command is run
    compile_commands.json   : compile data, as well from the place where command is run
EOF
}

get_files_to_check()
{
    local files
    local endlist=()
    local file_with_ignores="$1"
    files=$(git diff -U0 --name-only remotes/origin/${CHANGE_TARGET}...HEAD --diff-filter='d')
    for file in ${files}; do
        if [[ ${file} =~ ^.*\.(cpp|hpp|c|h|cxx|gcc|cc)$ ]] && shouldnt_ignore "${file}" > "$file_with_ignores"; then
            endlist+=("$file")
        fi
    done
    echo "${endlist[*]}"
}

# if not exists create
# could be in better place than tmp
get_compile_commands()
{
    product=$1
    [[ -f build-$product-linux-Debug/compile_commands.json ]] || ./configure.sh $product linux debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 > /dev/null 2>&1
    cp build-$product-linux-Debug/compile_commands.json /tmp/compile_commands.json
    sed -i 's|-static-libasan||g'     /tmp/compile_commands.json
    sed -i 's|-Wno-literal-suffix||g' /tmp/compile_commands.json
}

main()
{
    if [ $# -ne 1 ]; then
        help
        exit 0
    fi
    echo "Target branch: ${CHANGE_TARGET}"

    local files_to_check
    local ignore_files
    ignore_files=$(mktemp /tmp/clang_check.XXXXXX)

    files_to_check=$(get_files_to_check "$ignore_files")
    cat "$ignore_files" && rm "$ignore_files"

    if [[ -z $files_to_check ]]; then
        echo "no files to check"
        exit 0
    fi

    # get the stage
    verify_clang_format_version
    get_compile_commands "$1"
    # run tidy
    git diff -U0 --no-color remotes/origin/${CHANGE_TARGET}...HEAD $files_to_check | "$(get_clang_tidy)" -p 1 -path=/tmp/
}

main "$1"
