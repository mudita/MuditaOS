#!/bin/bash -e
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

usage() {
cat << ==usage
Usage: $(basename $0) image_path build_dir [boot.bin_file]
    image_path    - Destination image path name e.g., PurePhone.img
    sysroot       - product's system root e.g., build-rt1051-RelWithDebInfo/sysroot
    boot.bin_file - optional for linux image - name of the boot.bin file (for different targets)
==usage
}

if [[ ( $# -ne 2 ) && ( $# -ne 3 ) ]]; then
	echo "Error! Invalid argument count"
	usage
	exit -1
fi

IMAGE_NAME=$(realpath $1)
SYSROOT=$(realpath $2)
BIN_FILE=$3

if [ ! -d "$SYSROOT" ]; then
	echo "Error! ${SYSROOT} is not a directory"
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

GENLFS=$(realpath $(find $BUILDDIR -type f -iname genlittlefs -executable -print -quit))
if [ -z ${GENLFS} ]; then
    echo "Error: Unable to find genlilttlefs..."
    exit -1
fi

#Number of sectors in the phone EMMC card
DEVICE_BLK_COUNT=30621696
DEVICE_BLK_SIZE=512
# Partition sizes in sector 512 units
PART1_START=2048
PART1_SIZE=2097152
PART2_START=$(($PART1_START + $PART1_SIZE))
PART2_SIZE=$PART1_SIZE
PART3_START=$(($PART2_START + $PART2_SIZE))
PART3_SIZE=$(($DEVICE_BLK_COUNT - $PART1_SIZE - $PART2_SIZE - $PART1_START))

truncate -s $(($DEVICE_BLK_COUNT * $DEVICE_BLK_SIZE)) $IMAGE_NAME
sfdisk $IMAGE_NAME << ==sfdisk
label: dos
label-id: 0x09650eb4
unit: sectors

/dev/sdx1 : start=    $PART1_START,  size=    $PART1_SIZE, type=b, bootable
/dev/sdx2 : start=    $PART2_START,  size=    $PART2_SIZE, type=9e
/dev/sdx3 : start=    $PART3_START,  size=    $PART3_SIZE, type=9e
==sfdisk


# Format FAT partitions
PART1="$IMAGE_NAME@@$(($PART1_START * $DEVICE_BLK_SIZE))"
mformat -i "$PART1" -F -T $PART1_SIZE -M $DEVICE_BLK_SIZE -v MUDITAOS

if [ ! -d "${SYSROOT}/sys" ]; then
	echo "Fatal! Image folder sys/ missing in build. Check build system."
	exit -1
fi
cd "${SYSROOT}/sys"

#Copy FAT data
CURRENT_DATA="assets country-codes.db Luts.bin"

mmd -i "$PART1" ::/current
mmd -i "$PART1" ::/current/sys
mmd -i "$PART1" ::/updates

for i in $CURRENT_DATA; do
	f="current/$i"
	if [ -f "$f" -o -d "$f" ]; then
		mcopy -s -i "$PART1" $f ::/current/
	else
		echo "Error! Unable to copy item: $f"
		exit 1
	fi
done

if [[ -n "${BIN_FILE}" && -f "${BIN_FILE}" ]]; then
	mcopy -v -s -i "$PART1" ${BIN_FILE} ::/current/boot.bin
else
	echo "Warning! Missing boot.bin"
	echo "(it's fine for a Linux build)"
fi

mcopy -s -i "$PART1" .boot.json ::
mcopy -s -i "$PART1" .boot.json.crc32 ::

#Littlefs generate image
echo $(pwd)
$GENLFS --image=$IMAGE_NAME --block_size=32768  --overwrite  --partition_num=3 -- user/*
$GENLFS --image=$IMAGE_NAME --block_size=4096  --overwrite  --partition_num=2

# back to previous dir
cd -
sync
