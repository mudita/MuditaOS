#!/bin/bash
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# set -eo pipefail
if [ ! -e config/common.sh ]; then
	echo "No config/common.sh, run this script from git root"
	exit 1
fi

if [ ! $(which sfdisk) ]; then
	echo "No sfdisk, please install"
	exit 1
fi

if [ ! $(which parted) ]; then
	echo "No parted, please install"
	exit 1
fi

source config/common.sh
is_root=`id -u`
if [ "$1" == "" ] || [ $is_root != 0 ]; then
	echo "Refuse to work with no device. Runme as root [uid=$is_root]"
	echo -e "$0 <dev> [-f] [-p <product>]\n"
	exit 1
else
	dev=$1
fi

if [ "$2" == "-f" ]; then
	use_the_force=1
else
	use_the_force=0
fi

if [ "$2" == "-p" ]; then
	product_name=$3
elif [ "$3" == "-p" ]; then
	product_name=$4
fi

if [ "$product_name" == "" ]; then
	product_name=$PURE_PHONE_NAME
	echo "Using default product: $product_name"
elif [[ ! " ${PRODUCTS_LIST[@]} " =~ " ${product_name} " ]]; then
	echo "Unrecognized product: $3! Please provide one of the following products:"
	for i in "${PRODUCTS_LIST[@]}";
	do
		echo "$i"
	done
	exit 1
fi
echo "Using product: $product_name"

pcount=$(sfdisk --dump $dev | grep "start=" | wc -l)
is_mounted=$(grep $dev /etc/mtab | awk '{print $2}')
if [ "$is_mounted" == "" ]; then
	echo "$dev is not mounted"
else
	echo "$dev is mounted at $is_mounted, please unmount it first"
	exit 1
fi

if [ $pcount == 2 ] && [ $use_the_force == 0 ]; then
	echo "device $dev already has 2 partitions (use -f to force)"
	exit 1
else
	echo "$dev has $pcount partitions"
fi

echo "!!! $dev will be wiped, no recovery at this point"
echo "!!! press ENTER to continue or CTRL+C to cancel"
echo -ne "?"
read

echo "wipe disk header"
dd if=/dev/zero of=$dev bs=8192 count=8192
echo "create msdos partition table"
parted $dev mklabel msdos

sleep 1

echo "Sleep and re-probe disk"

sleep 1
partprobe

echo "write partition table"

sfdisk --wipe always $dev < config/products/$product_name/emmc_partition_table.dump

if [ $? != 0 ]; then
	echo "partitioning device $dev failed"
	exit 1
fi

part1=$(sfdisk $dev --dump | grep bootable | awk '{print $1}')
part2=$(sfdisk $dev --dump | tail -n 2 | head -n -1 | awk '{print $1}')

echo "create FATs"
echo "FAT: $MUDITAOS_PARTITION_PRIMARY $part1"
mkfs.vfat -n $MUDITAOS_PARTITION_PRIMARY $part1

echo "FAT: $MUDITAOS_PARTITION_RECOVERY $part2"
mkfs.vfat -n $MUDITAOS_PARTITION_RECOVERY $part2

echo "probe new partitions to OS"
partprobe
