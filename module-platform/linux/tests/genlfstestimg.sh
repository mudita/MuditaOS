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

if [ -z ${L_GIT_DIR+unbound} ] && [ -z $(export L_GIT_DIR=$(git rev-parse --show-toplevel 2>/dev/null)) ]; then
  export L_GIT_DIR=$(readlink -f "$(dirname "$0")/../../..");
fi

. "$L_GIT_DIR"/tools/locate_bins.sh
require_unprivileged truncate stat
require_commands sfdisk

truncate -s $IMAGE_SIZE $IMAGE_FILE

SECTOR_START=2048
SECTOR_END=$(( $(stat -c "%s" $IMAGE_FILE)/512 - $SECTOR_START))

maysudo sfdisk "$IMAGE_FILE" << ==sfdisk
label: dos
unit: sectors

/dev/sdz1 : start=$SECTOR_START, size=$SECTOR_END, type=9e
==sfdisk
pwd
./genlittlefs --image $IMAGE_FILE --block_size=32768  --overwrite  --partition_num 1 -- ${SYSROOT}/sys/.boot.json ${SYSROOT}/sys/current/assets/* module-platform/test_dir/*
