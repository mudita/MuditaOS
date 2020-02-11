#!/bin/bash
# set -e

if [ -z $1 ]; then
  BUILD_PATH=./build
  echo "copying from default: $BUILD_PATH"
  echo -e "usage: $0 <build-folder>\n"
else
  BUILD_PATH=$1
fi

PURE_DEV=/dev/disk/by-id/usb-NXP_SEMI_NXP_MASS_STORAGE_0123456789ABCDEF-0:0
PURE_DISK=`readlink -f $PURE_DEV`
# check if already mouted

MOUNT_ENTRY=$(grep "$PURE_DISK" /etc/mtab)
if [ $? -ne 0 ]; then
	>&2 echo "PurePhone not mounted. Mount or retry with Ozone. https://github.com/muditacom/PurePhone/blob/master/doc/running_on_phone.md#eMMC_upload"
	exit 2
fi
PURE_PATH=$(echo $MOUNT_ENTRY | awk -F " " '{print $2}')
# unespace
PURE_PATH=$(echo -e $PURE_PATH)


echo "PurePhone remove all files"
rm "$PURE_PATH"/* -r &>/dev/null || echo "PurePhone disk alread empty"
# sudo sync $PURE_DEV

echo "PurePhone copy build files"
cp -v $BUILD_PATH/boot.bin "$PURE_PATH"/  # | sed 's/'-\>'/'→'/g'
cp -v $BUILD_PATH/sys/* "$PURE_PATH"/ -r  # | sed 's/'-\>'/'→'/g'

# sudo sync $PURE_DEV # https://unix.stackexchange.com/a/345950
echo -e "PurePhone copied\n"

PURE_PARTITION=$(lsblk -nlp /dev/sda | tail +2 | awk '{print $1}')
# unmount the partition, but eject the disk
if $(udisksctl unmount -b "$PURE_PARTITION" > /dev/null ) || ! $(umount "$PURE_PARTITION" > /dev/null ); then
	echo "PurePhone unmouted"
	timeout --signal=SIGINT 1 udisksctl power-off -b $PURE_DISK
	echo "PurePhone ejected"
	echo "Done. You can reset PurePhone now"
else
	>&2 echo "Eject manually"
fi
