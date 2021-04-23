#!/bin/bash -e
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

usage() {
cat << ==usage
Usage: $(basename "$0") [image_path] [key=value...]
	image_path: Destination image path name for. ex eeprom.img
	key=value: pairs of key and value to put into json eeprom file
==usage
exit 1
}

if [ $# -lt 2 ]; then
	echo "Error! Invalid argument count"
	usage
	exit 1
fi

NVROM_FS_SIZE=32768
NVROM_LFS_BLOCK_SIZE=128
FACTORY_FILENAME="personalization.json"
IMAGE_NAME=$(realpath "$1")

#Find genlittlefs tool
GENLFS=$(find $SDIR -type f -iname genlittlefs -executable -print -quit)
if [ -z ${GENLFS} ]; then
    echo "Error: Unable to find genlilttlefs..."
    exit 1
fi

#Process all arguments in form key=value and create json string from it
shift
JSON="{"
for ARGUMENT in "$@"
do
  if [[ ${ARGUMENT} == *"="* ]]; then
    key=${ARGUMENT%=*}
    value=${ARGUMENT##*=}
    JSON+="\"$key\":\"$value\""
    JSON+=","
  else
      echo "Invalid argument: $ARGUMENT"
      usage
  fi
done
JSON=${JSON%?}
JSON+="}"
echo $JSON > "$FACTORY_FILENAME"

#Generate eeprom image
$GENLFS --image "$IMAGE_NAME" --block_size=$NVROM_LFS_BLOCK_SIZE --filesystem_size=$NVROM_FS_SIZE --overwrite -- $FACTORY_FILENAME

sync
