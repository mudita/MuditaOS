#!/bin/sh -x

# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# A script to find duplicated initialization code of global data

rootdir="$(realpath $(dirname $(realpath $0))/..)"
objfile="${1:-$(realpath ${rootdir}/build-rt1051-Debug/PurePhone.elf)}"

if [ ! -f ${objfile} ]; then
    echo "No such file: ${objfile}"
    exit 1
fi

arm-none-eabi-nm --print-size ${objfile} | grep _Z41__static_initialization_and_destruction_0ii | awk '{print $1" "$2}' | tr 'a-f' 'A-F' | while read -r symbol_start symbol_size;
do
    symbol_end=$(echo "obase=16;ibase=16;$symbol_start+$symbol_size" | bc)
    arm-none-eabi-objdump --start-address=0x$symbol_start --stop-address=0x$symbol_end -d -l $objfile
done | grep ".hpp:" | awk '{print $1}' | xargs realpath | sed 's,^'${rootdir}/',,' | sort | uniq -c | sort -n
