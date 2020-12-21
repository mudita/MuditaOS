#!/bin/bash -e
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

usage() {
cat << ==usage
Usage: $(basename $0) [image_dir] [assets_root_dir]
	image_dir Target disk image
	asset_root_dir Asset root directory
==usage
}

ASSETS_DIR="assets country-codes.db Luts.bin"

TEST_ITEMS="testfiles"

if [ $# -ne 2 ]; then
	echo "Error! Invalid argument count"
	usage
	exit -1
fi
IMAGE_NAME=$(realpath $1)
SRC_DATA=$(realpath $2)

if [ ! -d "$SRC_DATA" ]; then
	echo "Error! asset_root_dir is not a directory"
	usage
	exit -1
fi
_REQ_CMDS="sfdisk mtools awk truncate"
for cmd in $_REQ_CMDS; do
	if [ ! $(command -v $cmd) ]; then
		echo "Error! $cmd is not installed, please use 'sudo apt install' for install required tool"
		exit -1
	fi
done
#mtools version
_AWK_SCRIPT='
/[0-9]/ { 
	split($4,vers,"."); 
	if(vers[1]>=4 && vers[2]>=0 && vers[3] >= 24) { 
		print "true"; 
	}
	exit 0; 
}'
MTOOLS_OK=$(mtools --version | awk "${_AWK_SCRIPT}")

if [ ! $MTOOLS_OK ]; then
	echo "Invalid mtools version, please upgrade mtools to >= 4.0.24"
	exit -1
fi

truncate -s 16G $IMAGE_NAME
sfdisk $IMAGE_NAME << ==sfdisk
label: dos
label-id: 0x09650eb4
unit: sectors

/dev/sda1 : start=        2048, size=    28522496, type=b, bootable
/dev/sda2 : start=    28524544, size=     2097152, type=b
==sfdisk

PART1="$IMAGE_NAME@@1048576"
PART2="$IMAGE_NAME@@14604566528"
mformat -i "$PART1" -F -T 28522496 -v MUDITAOS
mformat -i "$PART2" -F -T 2097152 -v RECOVER
mmd -i "$PART1" ::/current
cd "$SRC_DATA"
for i in $ASSETS_DIR; do
    echo "copy: $i"
	mcopy -s -i "$PART1" $i ::/current/
done
mcopy -s -i "$PART1" user ::
mcopy -s -i "$PART1" .boot.json ::
mcopy -s -i "$PART1" .boot.json.crc32 ::
mmd -i "$PART1" ::/current/sys
mcopy -s -i "$PART1" data ::/current/sys

# Testing parts of files
for i in $TEST_ITEMS; do
	mcopy -s -i "$PART1" $i ::/current
done
mcopy -s -i "$PART1" sys/updates ::
cd -
