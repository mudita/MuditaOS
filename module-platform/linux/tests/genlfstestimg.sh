#!/bin/bash -e
#Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
	exit 1
fi
IMAGE_SIZE="$1"
IMAGE_FILE="$2"
SYSROOT="$3"
shift 3

# Validate image source folder
if [ ! -d "${SYSROOT}/system_a" ]; then
	echo "Fatal! Image folder 'system_a' missing in build. Check build system."
	exit 1
fi
if [ ! -d "${SYSROOT}/user" ]; then
	echo "Fatal! Image folder 'user' missing in build. Check build system."
	exit 1
fi

_REQ_CMDS="sfdisk truncate"
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

/dev/sdz1 : start=$SECTOR_START, size=$SECTOR_END, type=9e
==sfdisk
pwd
./genlittlefs --image $IMAGE_FILE --block_size=32768  --overwrite  --partition_num 1 -- ${SYSROOT}/user/boot.json ${SYSROOT}/system_a/assets/* module-platform/test_dir/*
