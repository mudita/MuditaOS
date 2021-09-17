#!/bin/bash -e
#Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
#For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

usage() {
cat << ==usage
Usage: $(basename $0) [image_size] [image_file] [sysroot] [files]...
	image_size Target disk image size
	image_file Target image name
	files Files to include in the image
==usage
}


if [ $# -lt 2 ]; then
	echo "Error! Invalid argument count"
	usage
	exit -1
fi
IMAGE_SIZE="$1"
IMAGE_FILE="$2"
SYSROOT="$3"
shift 3

if [ ! -d "${SYSROOT}/sys" ]; then
	echo "Invalid sysroot: ${SYSROOT}"
	exit -1
fi

_REQ_CMDS="sfdisk truncate mke2fs"
for cmd in $_REQ_CMDS; do
	if [ ! $(command -v $cmd) ]; then
		echo "Error! $cmd is not installed, please use 'sudo apt install' for install required tool"
		exit -1
	fi
done
truncate -s $IMAGE_SIZE $IMAGE_FILE

SECTOR_START=2048
SECTOR_END=$(( $(stat -c "%s" $IMAGE_FILE)/512 - $SECTOR_START))

sfdisk $IMAGE_FILE << ==sfdisk
label: dos
unit: sectors

/dev/sdz1 : start=$SECTOR_START, size=$SECTOR_END, type=83
==sfdisk

mke2fs \
  -F \
  -L 'user' \
  -N 0 \
  -E offset=$(($SECTOR_START*512)) \
  -O ^64bit \
  -O ^filetype \
  -O ^extents \
  -O ^flex_bg \
  -d "${SYSROOT}/sys/current/assets" \
  -m 0 \
  -r 1 \
  -t ext4 \
  "$IMAGE_FILE" \
;
#./genlittlefs --image $IMAGE_FILE --block_size=32768  --overwrite  --partition_num 1 -- ${SYSROOT}/sys/.boot.json ${SYSROOT}/sys/current/assets/* module-platform/test_dir/*
