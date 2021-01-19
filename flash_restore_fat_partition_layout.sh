#!/bin/bash -e
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

usage() {
cat << ==usage
Usage: $(basename $0) [disk_device]
        disk_device PurePhone disc device for ex. /dev/sdb
==usage
}

if [ $# -ne 1 ]; then
	echo "Error! Invalid argument count"
	usage
	exit -1
fi

BLK_DEV="$1"

if [ ! -b "$BLK_DEV" ]; then
	echo "Error: $BLK_DEV Not a block device"
	usage
	exit -1
fi

if [ ! -w "$BLK_DEV" ]; then
	echo "Error: Block device $BLK_DEV is not writable"
	usage
	exit -1
fi

_REQ_CMDS="sfdisk mtools awk"
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

sfdisk $BLK_DEV << ==sfdisk
label: dos
label-id: 0x09650eb4
unit: sectors

/dev/sdx1 : start=2048,  size=28522496, type=b, bootable
/dev/sdx2 : start=28524544,  size=2097152, type=b
==sfdisk
sleep 1
PART1="${BLK_DEV}1"
PART2="${BLK_DEV}2"
mformat -i "$PART1" -F -v MUDITAOS
mformat -i "$PART2" -F -v RECOVER
sync
echo "Old partition scheme layout restored. Please run ./flash_eMMC.sh script for add phone data"

