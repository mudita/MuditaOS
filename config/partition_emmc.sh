#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
if [ $is_root != 0 ]; then
	echo "Runme as root [uid=$is_root]"
	exit 1
fi

usage() { echo "Usage: $0 -d /dev/sdX -p <PurePhone|BellHybrid> [-f]" 1>&2; exit 1; }

use_the_force=0
while getopts "d:p:f" o; do
    case $o in
        d)
            dev=${OPTARG}
            ;;
        p)
            product_name=${OPTARG}
            ;;
        f)
            use_the_force=1
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${product_name}" ]; then
    usage
fi

echo "Using product: $product_name"

emmc_partition_table_dump_path=config/products/$product_name/emmc_partition_table.dump
if [ ! -f "$emmc_partition_table_dump_path" ]; then
	echo "ERROR! File \"config/products/$product_name/emmc_partition_table.dump\" not exist!"
	exit 1
fi

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
sfdisk --wipe always $dev < "$emmc_partition_table_dump_path"

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
