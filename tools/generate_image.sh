#!/bin/bash -e
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

usage() {
cat << ==usage
Usage: $(basename $0) image_path image_partitions sysroot
    image_path        - Destination image path name e.g., PurePhone.img
    image_partitions  - Path to image_partitions.map product-specific file
    sysroot           - product's system root e.g., build-rt1051-RelWithDebInfo/sysroot
==usage
}

if [[ ( $# -ne 3 ) ]]; then
	echo "Error! Invalid argument count $# args: $*"
	usage
	exit 1
fi

IMAGE_NAME=$(realpath $1)
IMAGE_PARTITIONS=$(realpath $2)
SYSROOT=$(realpath $3)

if [ ! -d "$SYSROOT" ]; then
	echo "Error! ${SYSROOT} is not a directory"
	usage
	exit 1
fi
_REQ_CMDS="sfdisk truncate mke2fs"
for cmd in $_REQ_CMDS; do
	if [ ! $(command -v $cmd) ]; then
		echo "Error! $cmd is not installed, please use 'sudo apt install' for install required tool"
		exit 1
	fi
done

source ${IMAGE_PARTITIONS}
DEVICE_BLK_SIZE=512

# Partition sizes in sector 512 units
PART1_START=2048
PART2_START=$(($PART1_START + $PART1_SIZE))
PART2_SIZE=$PART1_SIZE
PART3_START=$(($PART2_START + $PART2_SIZE))
PART3_SIZE=$(($DEVICE_BLK_COUNT - $PART1_SIZE - $PART2_SIZE - $PART1_START))

# Remove previous file
echo "Removing previous image file..."
rm -f $IMAGE_NAME
# Create image file and set its length to block device size
truncate -s $(($DEVICE_BLK_COUNT * $DEVICE_BLK_SIZE)) $IMAGE_NAME

# Create image
echo "Creating empty disk image..."
sfdisk $IMAGE_NAME << ==sfdisk
label: dos
label-id: 0x09650eb4
unit: sectors

/dev/sdx1 : start=    $PART1_START,  size=    $PART1_SIZE, type=83
/dev/sdx2 : start=    $PART2_START,  size=    $PART2_SIZE, type=83
/dev/sdx3 : start=    $PART3_START,  size=    $PART3_SIZE, type=83
==sfdisk

# Validate image source folder
if [ ! -d "${SYSROOT}/system_a" ]; then
	echo "Fatal! Image folder 'system_a' missing in build. Check build system."
	exit 1
fi
if [ ! -d "${SYSROOT}/user" ]; then
	echo "Fatal! Image folder 'user' missing in build. Check build system."
	exit 1
fi
cd "${SYSROOT}"

# Format partitions
# Disabling of the below options is required:
# ^64bit - 64-bit inodes are not supported by lwext4
# ^metadata_csum - metadata checksums have buggy implementation in lwext4

# Format ext4 system_a partition
echo "Formatting system_a partition as ext4..."
mke2fs \
  -F \
  -L 'system_a' \
  -N 0 \
  -E offset=$(($PART1_START*$DEVICE_BLK_SIZE)) \
  -O ^64bit \
  -O ^flex_bg \
  -O ^metadata_csum \
  -m 0 \
  -r 1 \
  -d 'system_a' \
  -t ext4 \
  "$IMAGE_NAME" \
  $((($PART1_SIZE*$DEVICE_BLK_SIZE)/1024))  > /dev/null

# Format ext4 system_b partition
echo "Formatting system_b partition as ext4..."
mke2fs \
  -F \
  -L 'system_b' \
  -N 0 \
  -E offset=$(($PART2_START*$DEVICE_BLK_SIZE)) \
  -O ^64bit \
  -O ^flex_bg \
  -O ^metadata_csum \
  -m 0 \
  -r 1 \
  -t ext4 \
  "$IMAGE_NAME" \
  $((($PART2_SIZE*$DEVICE_BLK_SIZE)/1024))  > /dev/null

# Format ext4 user partition
echo "Formatting user partition as ext4..."
mke2fs \
  -F \
  -L 'user' \
  -N 0 \
  -E offset=$(($PART3_START*$DEVICE_BLK_SIZE)) \
  -O ^64bit \
  -O ^flex_bg \
  -O ^metadata_csum \
  -m 0 \
  -r 1 \
  -d 'user' \
  -t ext4 \
  "$IMAGE_NAME" \
  $((($PART3_SIZE*$DEVICE_BLK_SIZE)/1024)) > /dev/null

echo "Image created!"

# Back to previous dir
cd - > /dev/null
sync
