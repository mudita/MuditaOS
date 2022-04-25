#!/bin/bash -e
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
#For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

if [ $# -ne 1 ]; then
	echo "Error! Invalid argument count"
	exit 1
fi
IMAGE_FILE_DIR="$1"

dd if=/dev/zero of=$IMAGE_FILE_DIR/test_disk.img bs=512 count=8
sfdisk $IMAGE_FILE_DIR/test_disk.img << ==sfdisk
,1,L
,1,L
,,L
==sfdisk

dd if=/dev/zero of=$IMAGE_FILE_DIR/test_disk_ext.img bs=512 count=16
sfdisk $IMAGE_FILE_DIR/test_disk_ext.img << ==sfdisk
,1,L
,,E
,1,L
,1,L
,1,L
,1,L
,1,L
,,L
==sfdisk

dd if=/dev/zero of=$IMAGE_FILE_DIR/test_disk_bad_tmp.img bs=512 count=16
sfdisk $IMAGE_FILE_DIR/test_disk_bad_tmp.img << ==sfdisk
,1,L
==sfdisk

# Modify parition table to simply broke it, which ten can be tested.
{ head -c 458 $IMAGE_FILE_DIR/test_disk_bad_tmp.img; printf '\xFF'; tail -c +460 $IMAGE_FILE_DIR/test_disk_bad_tmp.img; } > $IMAGE_FILE_DIR/test_disk_bad.img
rm $IMAGE_FILE_DIR/test_disk_bad_tmp.img

