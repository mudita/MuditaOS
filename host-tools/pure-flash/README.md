# PureFlash Guide
## Introduction
*PureFlash* is a command line tool for buring OS images into the flash memory using the Ecoboot MSC mode. 
The tool works only in the Linux system.

## Howto use
For invoke PureFlash we need to provide following arguments:
```
./pureflash filename blkdev
```
* filename - Is a path to the purephone image 
* blkdev - Is a major block device of the purephone.

## Example howto flash PurePhone image

* Download the MuditaOS release in the tar.gz format.
* Unpack the Release using the following command: `tar xf <release.tar.gz>`
* Switch the phone into the MSC mode *<4>*
* Check the phone block device using `lsblk` command
* Ensure that the any partitions from the Pure disks are not mounted using `mount` command
* If any partition from the Pure disk is mounted please umount partition using the `umount <path>` command, where the *<path>* is a mount point path
* Flash the phone using the following command `pureflash PurePhone.img </dev/sdX>` where *sdX* is a Pure block device identifier