Virtual File System for PurePhone
-----------------

# What does this module provide?

We use POSIX calls wrapper to have proper filesystem experience and it supports C++17 filesystem library.

# File systems in use

Right now we are able to use 3 file systems:

- FAT-FS
- Ext4 with 32 bit inodes.
- LittleFS

Curently only the FAT and EXT4 filesystems are used on the mass storage media.

# PurePhone partitions

PurePhone uses old MBR partitioning scheme and it has  3 flash partitions:

1. FAT for the phone firmware (**ReadOnly**)
2. EXT4 for the backup data
2. EXT4 for the user data

EXT4 partition is needed to assert minimal to possibly no data loss in unexpected shutdown
LFS can be used as an alternative user partition. Currently it is not used by the MuditaOS except internal EEPROM memory.

# how to use PurePhone partitions

Pure phone on reboot on option 9 will present itself with 3 partitions via USB, all partitions should be automount.
## mounting FAT-FS and EXT4 partitions

1. check which devices are **PurePhone** block devices ie with: `dmesg` or `lsblk`
2. mount them with i.e `udisksctl mount -b /dev/sdbX` where X is number of block device

## mounting LittleFS partitions

For LittleFS you will have to use fuse to mount partition. Tool is already build with release and is ready to use:

1. Prepare mount point i.e. create catalog `mkdir mymount`
2. Mount: `./lfsfuse --block_size=32768 /dev/sdX mymount` where sdbx is device where you expect LFS partition
3. Enter here and play with data
4. Umount and eject the device i.e. with: `udisksctl power-off -b /dev/sdb`
