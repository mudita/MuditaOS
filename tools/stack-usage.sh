#!/usr/bin/bash

# make sure, that GENERATE_STACK_USAGE option has been enabled in cmake configuration

builddir=$1
minsize=$2
pattern=$3

if [ -z "$builddir" ]; then
    echo "usage $0 builddir [minsize] [file name pattern]"
    exit 1
fi

if [ -z $minsize ]; then
    minsize=0
fi

if [ -n "$pattern" ]; then
    FILTER="| egrep $pattern"
fi

eval find $builddir -name "*.su" $FILTER | xargs awk '{if ( $(NF-1) > '$minsize') print $(NF-1)" "$0}' | sort -n
