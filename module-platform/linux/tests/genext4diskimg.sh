#!/bin/bash -e
#Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
#For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

usage() {
cat << ==usage
Usage: $(basename $0) [image_size] [image_file] [sysroot]
	image_size Target disk image size in bytes
	image_file Target image name
        sysroot    Source directory with files
==usage
}

if [ $# -lt 2 ]; then
	echo "Error! Invalid argument count"
	usage
	exit 1
fi
IMAGE_SIZE="$1"
IMAGE_FILE="$2"
SYSROOT="$3"
shift 3

if [ ! -d "${SYSROOT}" ]; then
	echo "Invalid sysroot: ${SYSROOT}"
	exit 1
fi

_REQ_CMDS="sfdisk truncate mke2fs"
for cmd in $_REQ_CMDS; do
	if [ ! $(command -v $cmd) ]; then
		echo "Error! $cmd is not installed, please use 'sudo apt install' for install required tool"
		exit 1
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

#Generate image
mke2fs \
  -F \
  -L 'user' \
  -N 0 \
  -E offset=$(($SECTOR_START*512)) \
  -O ^64bit \
  -O ^flex_bg \
  -O ^metadata_csum \
  -d "${SYSROOT}" \
  -m 0 \
  -r 1 \
  -t ext4 \
  "$IMAGE_FILE" \
;
