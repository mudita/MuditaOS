#!/bin/bash
# set -e

source config/common.sh

if [ -z $1 ]; then
  BUILD_PATH=./build-rt1051-Debug
  echo "copying from default: $BUILD_PATH"
  echo -e "usage: $0 <build-folder>\n"
else
  BUILD_PATH=$1
fi

check_target_rt1051 "$BUILD_PATH"

PURE_DEV=/dev/disk/by-id/usb-NXP_SEMI_NXP_MASS_STORAGE_0123456789ABCDEF-0:0-part1
PURE_DEV_RECOVER=/dev/disk/by-id/usb-NXP_SEMI_NXP_MASS_STORAGE_0123456789ABCDEF-0:0-part2
PURE_DISK=`readlink -f $PURE_DEV`
PURE_DISK_RECOVER=`readlink -f $PURE_DEV_RECOVER`
# check if already mouted

MOUNT_ENTRY_RECOVER=$(grep "$PURE_DISK_RECOVER" /etc/mtab)
MOUNT_ENTRY=$(grep "$PURE_DISK" /etc/mtab)
if [ $? -ne 0 ]; then
	>&2 echo "PurePhone not mounted. Mount or retry with Ozone. https://github.com/muditacom/PurePhone/blob/master/doc/running_on_phone.md#eMMC_upload"
	exit 2
fi

PURE_PATH=$(echo $MOUNT_ENTRY | awk -F " " '{print $2}')
# unespace
PURE_PATH_CURRENT=$(echo -e $PURE_PATH)/current
PURE_PATH_ROOT=$(echo -e $PURE_PATH)

echo "PurePhone remove all files"
rm -rf "$PURE_PATH_ROOT"/* -r &>/dev/null || echo "PurePhone disk alread empty"
rm -f "$PURE_PATH_ROOT"/.boot.ini
rm -f "$PURE_PATH_ROOT"/.boot.ini.crc32

echo "Create directories"
mkdir -p $PURE_PATH_ROOT/$PURE_OS_CURRENT
mkdir -p $PURE_PATH_ROOT/$PURE_OS_PREVIOUS
mkdir -p $PURE_PATH_ROOT/$PURE_OS_FACTORY

# sudo sync $PURE_DEV

echo "PurePhone copy build files"
cp -v $BUILD_PATH/boot.bin "$PURE_PATH_CURRENT"/  # | sed 's/'-\>'/'→'/g'

for file in $IMAGE_FILES; do
	echo Copying $file
	cp -vr $BUILD_PATH/$file "$PURE_PATH_CURRENT"/
done

for file in $IMAGE_MOVE_TO_ROOT; do
	echo "Moving $PURE_PATH_CURRENT/$file to $PURE_PATH_ROOT"
	mv "$PURE_PATH_CURRENT"/"$file" "$PURE_PATH_ROOT"/
done

# sudo sync $PURE_DEV # https://unix.stackexchange.com/a/345950
echo -e "PurePhone copied\n"

PURE_PARTITION=$(lsblk -nlp $PURE_DISK | tail +2 | awk '{print $1}')

if [ -z $PURE_PARTITION ]; then
       PURE_PARTITION=$PURE_DISK # it is formatted like so apparently
fi

if [ "$MOUNT_ENTRY" != "" ]; then
	echo "Unmounting disk: $PURE_DISK ($MOUNT_ENTRY)"
	udisksctl unmount -b $PURE_DISK > /dev/null
fi

if [ "$MOUNT_ENTRY_RECOVER" != "" ]; then
	echo "Unmounting disk: $PURE_DISK_RECOVER ($MOUNT_ENTRY_RECOVER)"
	udisksctl unmount -b $PURE_DISK_RECOVER > /dev/null
fi

sleep 1
echo "PurePhone unmouted"
echo "Ejecting USB"
timeout --signal=SIGINT 1 udisksctl power-off -b $PURE_DISK
echo
echo "Reset your phone now"
