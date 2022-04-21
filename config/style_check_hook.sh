#!/bin/bash 
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set -e
set -o pipefail
# trap 'echo ERROR on line: $LINENO running command: $BASH_COMMAND ; trap - EXIT; exit $?' EXIT

# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved. 
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# taken from: https://raw.githubusercontent.com/andrewseidl/githook-clang-format/master/clang-format.hook
# - with just added check for cpp file & check for proper version of clang-format
# - with use of clang-format diff.py - for changed chunk changes only
# might be worth to consider: https://github.com/Sarcasm/run-clang-format

# shellcheck source=../config/format-config.sh
# shellcheck source=../config/clang/colors.sh
# shellcheck source=../config/clang/common.sh

L_GIT_DIR=$(git rev-parse --show-toplevel)
source "${L_GIT_DIR}/config/format-config.sh"
source "${L_GIT_DIR}/config/clang/colors.sh"
source "${L_GIT_DIR}/config/clang/clang-common.sh"
CHANGE_TARGET=${CHANGE_TARGET:-master}


# if autoformatting was disabled by user - then ignore this commit hook
if [ "$DISABLE_AUTO_FORMATTING" -eq 1 ]; then
    [[ $VERBOSE ]] && echo "auto formatting is disabled"
    exit 0
fi

verify_clang_format_version

L_CLANG_DIFF_TOOL=$(get_clang_format)
if ! [[ $L_CLANG_DIFF_TOOL ]] || [[ $L_CLANG_DIFF_TOOL == "" ]]; then
    [[ $VERBOSE ]] && echo "clang tool not found"
    exit 1
fi


check_file() {
    file="${1}"
    last_commit="${2}"
    if [ -f "${file}" ]; then
        #[[ $VERBOSE ]] && echo -en "Checking: \e[33m${file}\e[0m :\t"
        results["${file}"]="${OK}"
        case ${last_commit} in
        "True")
            if [[ ${FIX,,} == "true" ]]; then
                git diff -U0 --no-color remotes/origin/"${CHANGE_TARGET}"...HEAD "${file}" | ${L_CLANG_DIFF_TOOL} -style file -p1 -i
                STATUS=$(git status --short -- "${file}")
                if [ -n "${STATUS}" ]; then
                    git add "${file}"
                    results["${file}"]="${FIXED}";
                fi
            else
                OUT=$(git diff -U0 --no-color remotes/origin/"${CHANGE_TARGET}"...HEAD "${file}" | ${L_CLANG_DIFF_TOOL} -style file -p1 )
                if [ -n "${OUT}" ]; then
                    results["${file}"]="${ERROR}"
                    return 1
                fi
            fi
            ;;
        "Stage")
            if [[ ${FIX,,} == "true" ]]; then
                git diff -U0 --no-color --cached "${file}" | ${L_CLANG_DIFF_TOOL} -style file -p1 -i
                STATUS=$(git status --short -- "${file}")
                if [ -n "${STATUS}" ]; then
                    git add "${file}"
                    results["${file}"]="${FIXED}";
                fi
            else
                OUT=$(git diff -U0 --no-color --cached "${file}" | ${L_CLANG_DIFF_TOOL} -style file -p1)
                if [ -n "${OUT}" ]; then
                    results["${file}"]="${ERROR}"
                    return 1
                fi
            fi
            ;;
        *)
            OUT=$(git diff -U0 --no-color --cached "${file}" | ${L_CLANG_DIFF_TOOL} -style file -p1 )
            if [[ -n ${OUT} ]]; then
                results["${file}"]="${ERROR}"
                return 1
            fi
            ;;
        esac
    fi
    return 0
}

function help() {
        echo "Runs clang-format on source files"
        cat <<- EOM
		usage:
		   $0 [option]
		        --about             does nothing
		        --last              checks current branch against origin/master - doesn't fix
		        --branch-fix        checks current branch against origin/master and fixes errors - you have to 'git add' and 'git commit' them
		        --fix               fix style in currently staged files (ignores user.fixinstage variale), this is usefull for manual style applying
		
		If you would like to automatically fix style before commit
		add to your git config "user.fixinstage" variable with value "True"
		by calling:
		    git config user.fixinstage True
		EOM
}


case "${1}" in
    --about|--help )
        help
        FILES=''
        LAST=""
        exit 0
        ;;
    --last)
        FILES=$(git diff --name-only remotes/origin/"${CHANGE_TARGET}"...HEAD)
        LAST="True"
        FIX="false"
        ;;
    --branch-fix)
        FILES=$(git diff --name-only remotes/origin/"${CHANGE_TARGET}"...HEAD)
        LAST="True"
        FIX="true"
        ;;
    --fix)
        FILES=$(git diff-index --cached --name-only HEAD)
        LAST="Stage" 
        FIX=true
        ;;
    *)
        if [[ $# -ne 0 ]]; then
            echo "unknown parameters: '$*'"
            help
            exit 1
        fi
        FILES=$(git diff-index --cached --name-only HEAD)
        LAST="Stage" 
        FIX=$([[ $(git config user.fixinstage) ]] && git config user.fixinstage || echo "false")
        FIX=${FIX:-false}
        ;;
esac

declare -A results

EXIT_CODE=0
for file in ${FILES}; do
    if [[ ${file} =~ ^.*\.(cpp|hpp|c|h|cxx|gcc|cc)$ ]] && shouldnt_ignore "${file}"; then
        check_file "${file}" ${LAST} || RESULT=$?
        if [[ ${RESULT} -eq 1 ]]; then
            EXIT_CODE=1
        fi
    fi
done

for FILE in "${!results[@]}"
do
    echo -e "  ${FILE} : ${results[${FILE}]}"
done

echo "exit: ${EXIT_CODE}"
exit ${EXIT_CODE}

