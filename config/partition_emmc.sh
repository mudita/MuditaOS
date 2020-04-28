#!/bin/bash
# set -e

is_root=`id -u`
if [ "$1" == "" ] || [ $is_root != 0 ]; then
	echo "Refuse to work with no device. Runme as root [uid=$is_root]"
	echo -e "$0 <dev> [-f]\n"
	exit 1
else
	dev=$1
fi

if [ "$2" == "-f" ]; then
	use_the_force=1
else
	use_the_force=0
fi

pcount=$(sfdisk --dump $dev | grep "start=" | wc -l)
is_mounted=$(grep $dev /etc/mtab)
if [ $? != 0 ]; then
	echo "$dev is not mounted"
fi

if [ $pcount == 2 ] && [ $use_the_force == 0 ]; then
	echo "device $dev already has 2 partitions"
	exit 1
else
	echo "$dev has $pcount partitions"
fi

sfdisk --wipe always $dev < emmc_partition_table.dump

if [ $? != 0 ]; then
	echo "partitioning device $dev failed"
	exit 1
fi

part1=$(sfdisk $dev --dump | grep bootable | awk '{print $1}')
part2=$(sfdisk $dev --dump | tail -n 1 | awk '{print $1}')

echo "create FATs"
echo "FAT: PUREOS"
mkfs.vfat -n PUREOS $part1
echo "FAT: BACKUP"
mkfs.vfat -n BACKUP $part2

echo "probe new partitions to OS"
partprobe
