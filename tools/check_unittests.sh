#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

root_dir="$(realpath $(dirname $(realpath $0))/..)"
build_dir="${root_dir}/build-linux-Debug"

ALL_TESTS_FILE=all_unittests
ALL_TESTS_PATH=${build_dir}/${ALL_TESTS_FILE}
ENABLED_TESTS_PATH=${root_dir}/enabled_unittests

END=0

trim() {
    local var="$*"
    # remove leading whitespace characters
    var="${var#"${var%%[![:space:]]*}"}"
    # remove trailing whitespace characters
    var="${var%"${var##*[![:space:]]}"}"   
    printf '%s' "$var"
}


# get all tests
${root_dir}/tools/get_unittests.sh ${ALL_TESTS_FILE} CHECK_OUT > /dev/null
TEMP_SOURCE=$(mktemp)
sed -e '/^[[:space:]]*#.*/d' ${ALL_TESTS_PATH} > $TEMP_SOURCE
source ${TEMP_SOURCE}

# get enabled tests
sed -e '/^[[:space:]]*#.*/d' ${ENABLED_TESTS_PATH} > $TEMP_SOURCE
source ${TEMP_SOURCE}

declare -a BINARY_NOT_RUN
declare -A TESTS_NOT_RUN
echo "--tests--"
for BINARY in ${!CHECK_OUT[@]}
do
    #echo BINARY:$BINARY
    #echo "TESTS_LIST[${BINARY}]: ${TESTS_LIST[${BINARY}]}" 
    if [[ -n "${TESTS_LIST[${BINARY}]}" ]]; then
        #echo ""
        readarray -d\; -t TESTS_FROM_ALL < <(echo ${CHECK_OUT["${BINARY}"]} | tr "\n" " ")
        #declare -p TESTS_FROM_ALL
        #echo "NO: ${#TESTS_FROM_ALL[@]}"
        I=0
        declare -a TEST_NOT_RUN
        while [[ ${I} -lt ${#TESTS_FROM_ALL[@]} ]]; 
        do
            CURRENT_TEST=$(trim ${TESTS_FROM_ALL[${I}]})
            if [[ -n "$CURRENT_TEST" ]]; then
                #if [[ ${BINARY} == "catch2-gui-text" ]]; then
                #    echo "--- CURRENT_TEST: ${CURRENT_TEST}"
                #fi
                CHK=$(echo ${TESTS_LIST["${BINARY}"]} | grep -F "${CURRENT_TEST}")
                #echo "#@#@#@# $CHK #@#@##"
                if [[ -z "${CHK}" ]]; then
                    #echo "!!!!!!!!!!${CURRENT_TEST}!!!!!!!!!"
                    TEST_NOT_RUN+=("${CURRENT_TEST}")
                fi
            fi
            I=$(( $I + 1 ))
        done
        if [[ "${#TEST_NOT_RUN[@]}" -gt 0 ]]; then
            #echo "!@@!@!@!@!@! ${#TEST_NOT_RUN[@]} @!#!@#!@#!"
            #declare -p TEST_NOT_RUN
            I=0
            TEST_NOT_RUN_STR=""
            while [[ $I -lt ${#TEST_NOT_RUN[@]} ]]
            do
                #TEST_NOT_RUN_STR="${TEST_NOT_RUN[${I}]};${TEST_NOT_RUN_STR}"
                TEST_NOT_RUN_STR="${TEST_NOT_RUN_STR};${TEST_NOT_RUN[${I}]}"
                I=$(( I + 1 ))
            done
            TESTS_NOT_RUN["${BINARY}"]="${TEST_NOT_RUN_STR}"
            unset TEST_NOT_RUN
        fi
    else 
        #echo "######## $BINARY ########"
        BINARY_NOT_RUN+=($BINARY)
    fi
done

if [[ -n "${BINARY_NOT_RUN}" ]]; then
    END=1
    I=0
    echo "binaries not run:"
    while [[ $I -lt ${#BINARY_NOT_RUN[@]} ]]
    do
        echo "  - ${BINARY_NOT_RUN[$I]}"
        I=$(( $I + 1 ))
    done
fi
pushd ${build_dir} > /dev/null
BINARIES_IN_BUILD=( `ls -1 catch2*` )
popd > /dev/null
BINARIES_IN_DECLARED=( ${!TESTS_LIST[@]} )

if [[ ${#BINARIES_IN_BUILD[@]} -ne ${#BINARIES_IN_DECLARED[@]} ]]; then
    END=1
    echo "not all binaries were build:"
    printf '\t- %s\n' ${BINARIES_IN_BUILD[@]} ${BINARIES_IN_DECLARED[@]} | sort | uniq -u
fi
#declare -p TESTS_NOT_RUN
#echo ${#TESTS_NOT_RUN[@]}
if [[ "${#TESTS_NOT_RUN[@]}" -gt 0 ]]; then
    END=1
    echo "Tests not run:"
    for BINARY in ${!TESTS_NOT_RUN[@]}
    do
        echo -n "  - Binary: ${BINARY}"
        echo "${TESTS_NOT_RUN[${BINARY}]}" | sed 's/;/\n\t- /g'
    done
fi

case $END in
    "0") echo "OK";;
    "*") echo "ERROR";;
esac

exit $END
