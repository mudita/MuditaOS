#!/bin/bash -e
usage() {
cat << ==usage
Usage: $(basename $0) [image_dir] [assets_root_dir]
	image_dir Target disk image
	asset_root_dir Asset root directory
==usage
}

ASSETS_DIR="assets country-codes.db Luts.bin"

TEST_ITEMS="testfiles"

if [ $# -ne 2 ]; then
	echo "Error! Invalid argument count"
	usage
	exit -1
fi
IMAGE_NAME=$(realpath $1)
SRC_DATA=$(realpath $2)

if [ ! -d "$SRC_DATA" ]; then
	echo "Error! asset_root_dir is not a directory"
	usage
	exit -1
fi

truncate -s 16G $IMAGE_NAME
sfdisk $IMAGE_NAME << ==sfdisk
label: dos
label-id: 0x09650eb4
unit: sectors

/dev/sda1 : start=        2048, size=    28522496, type=b, bootable
/dev/sda2 : start=    28524544, size=     2097152, type=b
==sfdisk

PART1="$IMAGE_NAME@@1048576"
PART2="$IMAGE_NAME@@14604566528"
mformat -i "$PART1" -F -T 28522496 -v PUREOS
mformat -i "$PART2" -F -T 2097152 -v RECOVER
mmd -i "$PART1" ::/current
cd "$SRC_DATA"
for i in $ASSETS_DIR; do
	mcopy -s -i "$PART1" $i ::/current/
done
mcopy -s -i "$PART1" user ::
mcopy -s -i "$PART1" .boot.json ::
mcopy -s -i "$PART1" .boot.json.crc32 ::

# Testing parts of files
for i in $TEST_ITEMS; do
	mcopy -s -i "$PART1" $i ::/current
done
mcopy -s -i "$PART1" sys/updates ::

cd -
