vfs for PurePhone
-----------------

# What does this module provide?

We use POSIX calls wrapper to have proper filesystem experience.

# File systems in use

Right now we are able to use 2 file systems
- FAT-FS
- LittleFS

# PurePhone partitions

PurePhone has 3 flash partitions:
1. for update and backup
2. FAT for phone data
3. LittleFS for user data

LittleFS partition is needed to assert minimal to possibly no data loss in unexpected shutdown

# how to use PurePhone partitions

Pure phone on reboot on option 4 will present itself with 3 partitions via USB, the first two should be auto mount.

## mounting FAT-FS partitions

1. check which devices are PurePhone block devices ie with: `dmesg` or `lsblk`
2. mount them with i.e `udisksctl mount -b /dev/sdbX` where X is number of block device

## mounting LittleFS partitions

For LittleFS you will have to use fuse to mount partition. Tool is already build with release and is ready to use:
1. Prepare mount point i.e. create catalog `mkdir mymount`
2. Mount: `./lfsfuse --block_size=32768 /dev/sdb3 mymount` where sdb3 is device where you expect Pure LittleFS partition
3. Enter here and play with user data
4. Umount and eject the device i.e. with: `udisksctl power-off -b /dev/sdb`
5. Phone will reboot with changed data
