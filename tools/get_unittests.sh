#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

root_dir="$(realpath $(dirname $(realpath $0))/..)"
build_dir="${root_dir}/build-linux-Debug"

function help() {
    cat <<- EOF
		Usage: $0 tests_list [VAR_NAME]
	EOF
}


if [[ $# -lt 1 ]]; then
    help
    exit 1
fi

TESTS_FILE=$1

TESTS_LIST_VAR=${2:-TESTS_LIST}

function addHeader() {
cat <<- EOF
	# Enabled test list
	# test format
	# TESTS_LIST["catch2-<binary-name>"]=" 
	#        test 1;
	#        test 1; 
	#        test 3;
	#"
	# 
	# each test is in separate line
	# to disable test just rem it by putting # in front of it
	# 
	declare -A ${TESTS_LIST_VAR}
	
EOF
}

pushd ${build_dir}
addHeader > ${TESTS_FILE}

for TEST_BINARY in catch2-*
do
    echo "${TEST_BINARY}"
    echo "${TESTS_LIST_VAR}[\"${TEST_BINARY}\"]=\"" >> ${TESTS_FILE}
    
    readarray -t TESTS_LIST < <(./${TEST_BINARY} -l | grep -e "^  " | xargs -i{} echo {} )
    I=0
    while [[ $I -lt ${#TESTS_LIST[@]} ]]
    do
        echo -e "    ${TESTS_LIST[$I]};" >> ${TESTS_FILE}
        I=$(( $I + 1 ))
    done
    echo "\"" >> ${TESTS_FILE}
    echo "#---------" >> ${TESTS_FILE}
done
popd

