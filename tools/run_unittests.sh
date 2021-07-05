#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

root_dir="$(realpath $(dirname $(realpath $0))/..)"
build_dir="${root_dir}/build-linux-Debug"

run_logs=${build_dir}/ut_run_logs

function help() {
    cat <<- EOF
		Usage: $0 tests_list
	EOF
}

trim() {
    local var="$*"
    # remove leading whitespace characters
    var="${var#"${var%%[![:space:]]*}"}"
    # remove trailing whitespace characters
    var="${var%"${var##*[![:space:]]}"}"   
    printf '%s' "$var"
}

if [[ $# -ne 1 ]]; then
    help
    exit 1
fi

source ${root_dir}/config/common_scripsts_lib

TESTS_FILE=$1

if [[ ! -r ${TESTS_FILE} ]]; then
    echo "Cannot read: \"${TESTS_FILE}\""
    exit 2
fi

TEMP_SOURCE=$(mktemp)

sed -e '/^[[:space:]]*#.*/d' ${TESTS_FILE} > $TEMP_SOURCE

source ${TEMP_SOURCE}

pushd ${build_dir}
for TEST_BINARY in catch2-*
do
    echo "= ${TEST_BINARY} ="
    readarray -d\; -t TESTS < <(echo ${TESTS_LIST["${TEST_BINARY}"]} | tr "\n" " " )
    I=0
    while [[ $I -lt ${#TESTS[@]} ]]
    do
        CUR_TEST=$( trim ${TESTS[$I]}) 
        if [[ -n "${CUR_TEST}" ]]; then
            echo -en "${TEST_BINARY} \"${CUR_TEST}\":"
            ./${TEST_BINARY} "${CUR_TEST}" --use-colour=yes > ${run_logs}
            RESULT=$?

            if [[ ${RESULT} -eq 0 ]]; then
                echo -e "${OK}"
            else
                echo -e "${ERROR}"
                cat ${run_logs}
                exit 1
            fi

            CHK_IF_RUN=`cat ${run_logs} | grep "No tests ran" || true` 
            echo "---$CHK_IF_RUN---"
            if [[ -n "${CHK_IF_RUN}" ]]; then
                echo "no such test: ${TEST_BINARY} ${CUR_TEST}"
                exit 1
            fi
        fi
        I=$(( $I + 1 ))
    done
done
popd
