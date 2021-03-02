#!/bin/bash
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

if [[ $BASH_VERSINFO -lt 4 ]]; then
    echo "Please update your bash to at last version 4"
	echo "your is: ${BASH_VERSINFO}"
    exit 4
fi

INVALID_CRC_FILE_NAME="invalid_crc.json" 

readarray -t sub_directories < <(ls -d */)

for dir in "${sub_directories[@]}"
do
	echo "Change directory to: $dir"
	cd "$dir"
	
	readarray -t js_files < <(find . -type f -name '*.json')
	
	for file in "${js_files[@]}"
	do
		CRC32=$(crc32 $file)
		echo -e "json file name: "$file "-> computed crc32: "$CRC32
		
		if [ $file == "./$INVALID_CRC_FILE_NAME" ]; then
			CRC32="$RANDOM"
			echo "overwrite computed crc with invalid random value: "$CRC32
		fi
		
		echo "${CRC32}" > "$file.crc32"
	done
	
	echo
	cd ..
done
