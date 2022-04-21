#!/bin/bash

# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# A script to find duplicated initialization code of global data

rootdir=$(realpath $(dirname $(realpath "${0}"))/..)
objfile=${1:-$(realpath "${rootdir}/build-rt1051-Debug/PurePhone.elf")}

if [ ! -f "${objfile}" ]; then
    echo "No such file: ${objfile}"
    exit 1
fi

type=$( file "${objfile}" | grep "ARM" )

if [[ -z "${type}" ]]; then
    echo "x86"
    OBJDUMP="objdump"
    NM="nm"
else
    echo "ARM"
    OBJDUMP="arm-none-eabi-objdump"
    NM="arm-none-eabi-nm"
fi

STATIC_SYMBOL="_Z41__static_initialization_and_destruction_0ii"

staticList=($(${NM} --print-size "${objfile}" | grep "${STATIC_SYMBOL}"|cut -f1,2 -d" "|tr " " ":" ))

echo "${#staticList[@]}"

I=0
statcListSize=${#staticList[@]}

searchRegexp='(.*\.hpp:[0-9]+)'
while [[ $I -lt $statcListSize ]]
do
    symbol=${staticList[${I}]}
    symbolStart="0x${staticList[${I}]%:*}"
    symbolSize="0x${staticList[${I}]#*:}"
    printf -v symbolEnd "0x%x" $(( ${symbolStart} + ${symbolSize} ))
    echo "${I}/${statcListSize}: ${symbolStart} - ${symbolEnd}"

    items=( $(${OBJDUMP} --start-address="${symbolStart}" --stop-address="${symbolEnd}" -d -l "${objfile}" ) )
    J=0
    while [[ ${J} -lt ${#items[@]} ]]
    do
        item=$( echo ${items[$J]} | cut -f1 -d" ")
        if [[ ${item} =~ ${searchRegexp} ]]; then
            paths="$paths\n${BASH_REMATCH[1]}"
        fi
        J=$(( $J + 1 ))
    done
    I=$(( $I + 1 ))
done

if [[ -n ${paths} ]]; then
    echo -e ${paths} | sort | uniq -c | sort -n
fi

