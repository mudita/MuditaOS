#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md


#time update doesn't work below
SCRIPT=$(readlink -f $0)
SCRIPT_DIR="$(dirname ${SCRIPT})"
REPO_ROOT="${SCRIPT_DIR%/*}"

#. ${SCRIPT_DIR}/bootstrap_config
. ${SCRIPT_DIR}/common_scripts_lib
. ${SCRIPT_DIR}/format-config.sh

#printVar SCRIPT
#printVar SCRIPT_DIR
#printVar REPO_ROOT

pushd ${REPO_ROOT} >> /dev/null

CURRENT_YEAR=$(date "+%Y")

LICENSE_CHEK="\(.*\)\(Copyright (c) 2017-\)\(.*\)\(, Mudita Sp. z.o.o. All rights reserved.\).*"
LICENSE_CHEK_BASH_RE="(.*)(Copyright \(c\) 2017-)(.*)(, Mudita Sp. z.o.o. All rights reserved.)"
LICENSE1="Copyright (c) 2017-${CURRENT_YEAR}, Mudita Sp. z.o.o. All rights reserved."
LICENSE2="For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md"

CHECK_ONLY="false"
EXIT_CODE=0

#file extension; comment;checker/replacer function
declare -A FileTypes=( 
            ["c_header"]="h;cppChecker;"
            ["c_source"]="c;cppChecker;"
            ["header"]="hpp;cppChecker;"
            ["source"]="cpp;cppChecker;"
            ["python"]="py;pythonChecker;"
            ["shell"]="sh;bashChecker;"
            ["sqlite"]="sql;sqlChecker;"
        )

function addEmptyLine() {
    LINE=$1
    CHECK_EMPTY=$(sed -n "${LINE}p" ${FILE})
    if [[ -n ${CHECK_EMPTY} ]]; then
        sed -i ${LINE}'s@^\(.*\)$@\n\1@' ${FILE}
    fi
}

function updateYear() {
    if [[ ${CHECK_LICENSE} =~ ${LICENSE_CHEK_BASH_RE} ]]; then
        sed -i "1,5s|${LICENSE_CHEK}|\1\2${CURRENT_YEAR}\4|g" ${FILE}
        echo -e "${YELLOW}Updated\e[0m"
    else 
        echo -e "${OK}"
    fi
}



function codeChecker(){
    CHECK_LICENSE=$(head -n5 ${FILE} | grep "${LICENSE_CHEK}")
    if [[ -z ${CHECK_LICENSE} ]]; then
        if [[ ${CHECK_ONLY} == "true" ]]; then
            echo -e "${ERROR}"
            EXIT_CODE=1
        else 
            sed -i "1i${LICENSE_LINE1}" ${FILE}
            sed -i "2i${LICENSE_LINE2}" ${FILE}
            addEmptyLine 3
            echo -e "${FIXED}"
        fi
    else 
        updateYear
    fi
}

function cppChecker() {
    FILE=$1
    LICENSE_LINE1="// ${LICENSE1}"
    LICENSE_LINE2="// ${LICENSE2}"
    codeChecker
}

function sqlChecker() {
    FILE=$1
    LICENSE_LINE1="-- ${LICENSE1}"
    LICENSE_LINE2="-- ${LICENSE2}"
    codeChecker
}

function scriptChecker(){
    CHECK_LICENSE=$( head -n5 ${FILE} | grep "${LICENSE_CHEK}")
    if [[ -z ${CHECK_LICENSE} ]]; then
        if [[ ${CHECK_ONLY} == "true" ]]; then
            echo -e "${ERROR}"
            EXIT_CODE=1
        else
            FIST_LINE=$(head -n1 ${FILE}|grep "$SHA_BANG_EXEC")
            if [[ -n "${FIST_LINE}" ]]; then
                sed -i "2i${LICENSE_LINE1}" ${FILE}
                sed -i "3i${LICENSE_LINE2}" ${FILE}
                addEmptyLine 4
            else 
                sed -i "1i${LICENSE_LINE1}" ${FILE}
                sed -i "2i${LICENSE_LINE2}" ${FILE}
                addEmptyLine 3
            fi
            echo -e "${FIXED}"
        fi
    else 
        updateYear
    fi

}
function pythonChecker(){
    FILE=$1
    LICENSE_LINE1="# ${LICENSE1}"
    LICENSE_LINE2="# ${LICENSE2}"
    SHA_BANG_EXEC="python"
    scriptChecker
    
} 

function bashChecker(){
    FILE=$1
    LICENSE_LINE1="# ${LICENSE1}"
    LICENSE_LINE2="# ${LICENSE2}"
    SHA_BANG_EXEC="bash"
    scriptChecker
}

SUBMODULES=( $(git submodule | cut -f3 -d" " ) )

function extractValues() {
    KEY=$1

    readarray -td ';' DATA <<< ${FileTypes[${KEY}]}

    FILE_EXT=${DATA[0]}
    CHECK_FUNCTION=${DATA[1]}
}

function excludePathFromFind(){
    # ignore submodule paths
    I=0
    SUB_COUNT=${#SUBMODULES[@]}
    while [[ $I -lt ${SUB_COUNT} ]];
    do 
        module=${SUBMODULES[$I]}
        NOT_PATH="${NOT_PATH} -not ( -path ./${module} -prune )"
        I=$(( $I + 1))
    done
    
    # ignore paths excluded from format
    I=0
    IGNORE_PATHS_COUNT=${#ignore_paths[@]}
    while [[ $I -lt ${IGNORE_PATHS_COUNT} ]]
    do
        path=${ignore_paths[$I]}
        if [[ -d ${path} ]]; then
            NOT_PATH="${NOT_PATH} -not ( -path ${path%*/} -prune )"
        fi
        I=$(( $I + 1 ))
    done
}

function findFiles() {
    # find files except submodule directories
    readarray -d '' FILES_TO_CHECK < <(find . ${NOT_PATH} -iname "*.${FILE_EXT}" -printf "%P\0")
}

function shouldnt_ignore() {
    FILE=$1
    for el in ${ignore_paths[@]}; do
        if [[ ${FILE}  =~ ^${el}.* ]]; then
            echo -e "${ORANGE}Ignore: ${FILE} checking due to: $el match!\e[0m"
            return 1
        fi
    done
    return 0
}

function help() {
    cat <<END
    check and update header files with copyright notice
    usage:
        $0 [--help | --check-only]
        $0 --hook [ --check-only]
                Run as git "pre-commith.hook", checks against files in stash
        $0 --ci 
                Run on CI, check files that are new to the current master
            
            --help              - this help message
            --check-only        - only checks, do not change a file

END
    exit 0
}

function hookMain() {
    parseArgs $@
    readarray -d '' FILES_TO_CHECK < <( ${GET_FILES_CMD} )

    for FILE_TYPE in "${!FileTypes[@]}" 
    do
        echo "=== ${FILE_TYPE} ==="
        extractValues ${FILE_TYPE}
        I=0
        while [[ $I -lt ${#FILES_TO_CHECK[@]} ]]
        do
            FILE=${FILES_TO_CHECK[$I]}
            if shouldnt_ignore ${FILE}; then
                if [[ ${FILE} =~ .*\.${FILE_EXT}$ ]]; then
                    echo -en "${FILE}:"
                    ${CHECK_FUNCTION} ${FILE}
                    git add ${FILE}
                fi
            fi
            I=$(( $I + 1 ))
        done
    done
    echo "Hook ${EXIT_CODE}"
    exit ${EXIT_CODE}
}

function standAloneMain() {
    NOT_PATH=""
    excludePathFromFind
    
    for FILE_TYPE in "${!FileTypes[@]}"
    do
        echo "=== ${FILE_TYPE} ==="
        extractValues ${FILE_TYPE}
        findFiles
        I=0
        while [[ $I -lt ${#FILES_TO_CHECK[@]} ]]
        do
            FILE=${FILES_TO_CHECK[$I]}
            if shouldnt_ignore ${FILE}; then
                echo -en "${I}/${#FILES_TO_CHECK[@]}:{${FILE}: "
                ${CHECK_FUNCTION} ${FILE}
            fi
            I=$(( $I + 1 ))
        done
    done
    echo "StandAlone: ${EXIT_CODE}"
    exit ${EXIT_CODE}
}

function parseArgs() {
    if [[ $# -ge 1 ]]; then
        case $1 in
            "--help")
                help
                ;;
            "--check-only")
                CHECK_ONLY=true
                ;;
            "--hook")
                GET_FILES_CMD="git diff --cached --name-only --diff-filter=AM -z ${against:-HEAD}"
                shift
                hookMain $@
                ;;
            "--ci")
                shift
                CHECK_ONLY=true
                GET_FILES_CMD="git diff --name-only --diff-filter=A -z remotes/origin/master...HEAD"
                hookMain $@
                ;;
            *)
            ;;
        esac
    fi
}
parseArgs $@
standAloneMain
