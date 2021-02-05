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
	# folowing caracters have to be escaped:
	# [],
	# \[ \] \,
	
	declare -A ${TESTS_LIST_VAR}
	
EOF
}

pushd ${build_dir}
addHeader > ${TESTS_FILE}

for TEST_BINARY in catch2-*
do
    echo "${TEST_BINARY}"
    echo "${TESTS_LIST_VAR}[\"${TEST_BINARY}\"]=\"" >> ${TESTS_FILE}

    #sed explaind
    #:a         - lablel 'a' current pattern space to jump to
    #N          - append next line to pattern space (wiht \n)
    #/\n  /     - search for this pattern
    #s/\n //    - delete "\n " (can be "replace")
    #ta         - if we changed something, jump to lable 'a'
    #P          - print part until new line
    #D          - delete part until new line
    TESTS_LIST_STR=$(./${TEST_BINARY} -l | grep -E "^  .*" | cut -f3- -d\  | sed -e ':a;N;/\n  /s/\n //;ta;P;D' )

    readarray -t TESTS_LIST < <( echo -e "$TESTS_LIST_STR" | tr "\n" "\0" | xargs -0 -i{} echo {} )

    I=0
    while [[ $I -lt ${#TESTS_LIST[@]} ]]
    do
        #sed cannot search for single ] to search for it we use ][ patter (and also look for closing ])
        echo -e "    ${TESTS_LIST[$I]};" | sed -e 's#[][,]#\\&#g' >> ${TESTS_FILE}
        I=$(( $I + 1 ))
    done
    echo "\"" >> ${TESTS_FILE}
    echo "#---------" >> ${TESTS_FILE}
done
popd

