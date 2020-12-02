#!/bin/bash
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# set -e

source config/common.sh

if [ -z $1 ]; then
  BUILD_PATH=./build-rt1051-Debug
  echo "copying from default: $BUILD_PATH"
  echo -e "usage: $0 <build-folder>\n"
else
  BUILD_PATH=$1
fi

if [ ! -d $BUILD_PATH ]; then
	echo "build directory: $BUILD_PATH does not exit"
	exit 2
fi

check_target_rt1051 "$BUILD_PATH"

MUDITAOS_DEV=/dev/disk/by-id/usb-NXP_SEMI_NXP_MASS_STORAGE_0123456789ABCDEF-0:0-part1
MUDITAOS_DEV_RECOVER=/dev/disk/by-id/usb-NXP_SEMI_NXP_MASS_STORAGE_0123456789ABCDEF-0:0-part2
MUDITAOS_DISK=`readlink -f $MUDITAOS_DEV`
MUDITAOS_DISK_RECOVER=`readlink -f $MUDITAOS_DEV_RECOVER`
# check if already mouted

if [ ! -f $BUILD_PATH/boot.bin ]; then
	echo
	echo "boot.bin does not exist in $BUILD_PATH, maybe you need to $ make in this dir"
	echo
	exit 2
fi

MOUNT_ENTRY_RECOVER=$(grep "$MUDITAOS_DISK_RECOVER" /etc/mtab)
MOUNT_ENTRY=$(grep "$MUDITAOS_DISK" /etc/mtab)
if [ $? -ne 0 ]; then
	>&2 echo "PurePhone not mounted. Mount or retry with Ozone. https://github.com/muditacom/PurePhone/blob/master/doc/running_on_phone.md#eMMC_upload"
	exit 2
fi
grep "[[:space:]]ro[[:space:],]" <<< $MOUNT_ENTRY
if [ $? -eq 0 ]; then
	>&2 echo "PurePhone mounted in readonly mode please unmount fs and check disc with command"
	>&2 echo "sudo fsck.vfat -a $(awk '{ print $1 }' <<< $MOUNT_ENTRY)"
	exit 3
fi
grep "[[:space:]]ro[[:space:],]" <<< $MOUNT_ENTRY_RECOVER
if [ $? -eq 0 ]; then
	>&2 echo "PurePhone mounted in readonly mode please unmount fs and check disc with command"
	>&2 echo "sudo fsck.vfat -a $(awk '{ print $1 }' <<< $MOUNT_ENTRY_RECOVER)"
	exit 4
fi

MUDITAOS_PATH=$(echo $MOUNT_ENTRY | awk -F " " '{print $2}')
# unespace
MUDITAOS_PATH_CURRENT=$(echo -e $MUDITAOS_PATH)/current
MUDITAOS_PATH_ROOT=$(echo -e $MUDITAOS_PATH)

echo "PurePhone remove all files"
rm -rf "$MUDITAOS_PATH_ROOT"/* -r &>/dev/null || echo "PurePhone disk alread empty"
rm -f "$MUDITAOS_PATH_ROOT"/.boot.json
rm -f "$MUDITAOS_PATH_ROOT"/.boot.json.crc32
rm -rf "$MUDITAOS_PATH_ROOT"/.Trash-1000

if [ "$(ls -A $MUDITAOS_PATH_ROOT)" ]; then
	echo "Couldn't delete all files in $MUDITAOS_PATH_ROOT leftover files:"
	find $MUDITAOS_PATH_ROOT -type f -printf "%f\n"
	exit 5
else
	echo "$MUDITAOS_PATH_ROOT is Empty"
fi

echo "Create directories"
mkdir -p $MUDITAOS_PATH_ROOT/$MUDITAOS_CURRENT
mkdir -p $MUDITAOS_PATH_ROOT/$MUDITAOS_PREVIOUS
mkdir -p $MUDITAOS_PATH_ROOT/$MUDITAOS_FACTORY
mkdir -p $MUDITAOS_PATH_ROOT/$MUDITAOS_UPDATES

# sudo sync $MUDITAOS_DEV

echo "PurePhone copy build files"
cp -v $BUILD_PATH/boot.bin "$MUDITAOS_PATH_CURRENT"/  # | sed 's/'-\>'/'→'/g'

for file in $IMAGE_FILES; do
	echo Copying $file
	cp -vr $BUILD_PATH/$file "$MUDITAOS_PATH_CURRENT"/
done

for file in $IMAGE_MOVE_TO_ROOT; do
	echo "Moving $MUDITAOS_PATH_CURRENT/$file to $MUDITAOS_PATH_ROOT"
	mv "$MUDITAOS_PATH_CURRENT"/"$file" "$MUDITAOS_PATH_ROOT"/
done

# sudo sync $MUDITAOS_DEV # https://unix.stackexchange.com/a/345950
echo -e "PurePhone copied\n"

MUDITAOS_PARTITION=$(lsblk -nlp $MUDITAOS_DISK | tail +2 | awk '{print $1}')

if [ -z $MUDITAOS_PARTITION ]; then
       MUDITAOS_PARTITION=$MUDITAOS_DISK # it is formatted like so apparently
fi

echo "Syncing data"
sync && sync && sync

if [ "$MOUNT_ENTRY" != "" ]; then
	echo "Unmounting disk: $MUDITAOS_DISK ($MOUNT_ENTRY)"
	udisksctl unmount -b $MUDITAOS_DISK > /dev/null
fi

if [ "$MOUNT_ENTRY_RECOVER" != "" ]; then
	echo "Unmounting disk: $MUDITAOS_DISK_RECOVER ($MOUNT_ENTRY_RECOVER)"
	udisksctl unmount -b $MUDITAOS_DISK_RECOVER > /dev/null
fi

sleep 1
echo "PurePhone unmouted"
echo "Ejecting USB"
timeout --signal=SIGINT 1 udisksctl power-off -b $MUDITAOS_DISK
echo "Reset your phone now"
