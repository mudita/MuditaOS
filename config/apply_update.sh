#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#
# It works when the phone is in USB-MSC mode, it will apply the .tar
# file provided as the -u parameter to the script, as the "current
# version on the Pure phone.
#
# Works on Linux and MacOS
#

UPDATE_FILE=""
PHONE_PARTITION_NAME="MUDITAOS"
CLEAN_PHONE=0
TMPDIR="flashmac"
OS=`uname -s`
PHONE_DEV=""
LINUX_DEV_FILE="usb-NXP_SEMI_NXP_MASS_STORAGE_0123456789ABCDEF-0:0-part1"

function get_phone_dev() {
	if [ "$OS" == "Darwin" ]; then
		diskutil list | grep $PHONE_PARTITION_NAME | awk '{print $6}'
	else
		readlink -f /dev/disk/by-id/$LINUX_DEV_FILE		
	fi
}

function eject_phone() {
	if [ "$OS" == "Darwin" ]; then
		diskutil eject $PHONE_DEV
	else
		udisksctl unmount -b $PHONE_DEV
		timeout --signal=SIGINT 1 udisksctl power-off -b $PHONE_DEV
	fi
}

print_help() {
	echo "Usage: $0 [OPTION]"
	echo "  -h print help"
	echo "  -u update file"
	echo "  -c clean before update"
	echo
	exit 1
}

while getopts ":hu:c" arg; do
	case "${arg}" in
		h)
			print_help
			;;
		u)
			UPDATE_FILE=$OPTARG
			;;
		c)
			CLEAN_PHONE=1
			;;
		*)
			print_help
			;;
	esac
done

if [ $OPTIND -eq 1 ]; then
	print_help
fi

shift $((OPTIND-1))
PHONE_DEV=$(get_phone_dev)

if [ "$PHONE_DEV" == "" ]; then
	echo "Can't find $PHONE_PARTITION_NAME device via diskutil, make sure that the phone is in USB-MSC mode"
	exit 1
fi

PHONE_MOUNT=`df -P | grep $PHONE_DEV | awk '{print $6}'`

if [ "$PHONE_MOUNT" == "" ]; then
	echo dev: $PHONE_DEV not mounted, do that yorself and re-run
	exit 1
fi

if [ ! -e $UPDATE_FILE ]; then
	echo "Update file $UPDATE_FILE does not exist"
	exit 1
fi

rm -rf $TMPDIR
mkdir $TMPDIR
tar -C $TMPDIR -xpf $UPDATE_FILE

if [ $CLEAN_PHONE == 1 ]; then
	echo "Clean phone requested"
	rm -rf $PHONE_MOUNT/current
	mkdir -p $PHONE_MOUNT/current
	sync
fi

echo "Copyind data"
cp $TMPDIR/boot.bin $PHONE_MOUNT/current/
cp $TMPDIR/Luts.bin $PHONE_MOUNT/current/
cp $TMPDIR/version.json $PHONE_MOUNT/current/
cp -r $TMPDIR/assets $PHONE_MOUNT/current/

echo "Ejecting phone from OS..."
eject_phone

