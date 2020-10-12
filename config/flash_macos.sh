#!/bin/bash
# set -eo piefail

UPDATE_FILE=""
PHONE_PARTITION_NAME="PUREOS"
CLEAN_PHONE=0
TMPDIR="flashmac"

function ex() {
	eval "$@"
	echo "[exec]: '$@'"
}

print_help() {
	echo "Usage: $0 [OPTION]"
	echo "  -h print help"
	echo "  -f update file"
	echo "  -c clean before update"
	echo
	exit 1
}

while getopts "hf:c" arg; do
	case "${arg}" in
		h)
			print_help
			;;
		f)
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
shift $((OPTIND-1))

PHONE_DEV=`diskutil list | grep $PHONE_PARTITION_NAME | awk '{print $6}'`
if [ "$PHONE_DEV" == "" ]; then
	echo "Can't find $PHONE_PARTITION_NAME device via diskutil, make sure that the phone is in USB-MSC mode"
	exit 1
fi

PHONE_MOUNT=`df -h | grep $PHONE_DEV | awk '{print $9}'`

if [ ! -d $PHONE_MOUNT ]; then
	echo dev: $PHONE_DEV mount: $PHONE_MOUNT is not a directory
	exit 1
fi

if [ ! -e $UPDATE_FILE ]; then
	echo "Update file $UPDATE_FILE does not exist"
	exit 1
fi

ex rm -rf $TMPDIR
ex mkdir $TMPDIR
ex tar -C $TMPDIR -xpf $UPDATE_FILE

if [ $CLEAN_PHONE == 1 ]; then
	echo "Clean phone requested"
	ex rm -rf $PHONE_MOUNT/current
	ex mkdir -p $PHONE_MOUNT/current
	ex sync
fi

ex cp $TMPDIR/boot.bin $PHONE_MOUNT/current/
ex cp $TMPDIR/Luts.bin $PHONE_MOUNT/current/
ex cp $TMPDIR/country-codes.db $PHONE_MOUNT/current/
ex cp $TMPDIR/version.json $PHONE_MOUNT/current/

ex cp -r $TMPDIR/assets $PHONE_MOUNT/current/
echo "Ejecting phone from OS..."
ex diskutil eject $PHONE_DEV
