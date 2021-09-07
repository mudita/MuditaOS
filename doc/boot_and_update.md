# How to boot Mudita Pure and create a storage partition

## Booting Mudita Pure

To follow these steps please make sure that you have the latest `ecoboot` bootloader on the phone. We recommend [version 1.0.4](https://github.com/mudita/ecoboot/releases/tag/1.0.4) which will work even if you manage to break the filesystems in the next steps (it's goot to be prepared).

1. `ecoboot` reads `/.boot.json` and `/.boot.json.crc32` and verifies if `crc32` in `/.boot.json.crc32` matches the actual `crc32` of `/.boot.json`

2. If `ecoboot` can't read `/.boot.json` and/or `/.boot.json.crc32` it tries to read `/.boot.json.bak` and `/.boot.json.bak.crc32` and verifies the checksum of `/.boot.json.bak`. If the `/.boot.json.bak` file passes the checksum test ecoboot should fix `/.boot.json` and `/.boot.json.crc32` files so that MuditaOS can pick up what version is booted (not implemented, ecoboot will fallback to booting /sys/current/boot.bin).

3. If both above steps fail, `ecoboot` reads `/sys/current/boot.bin` and loads it (failsafe)

4. `boot.json` contains the filename to load in a simple INI format

```bash
{
    "main": 
    {
        "ostype": "current",
        "imagename": "boot.bin"
    },
    "bootloader":
    {
        "version":"0.0.0"
    },
    "git": {}
}
  
```

There should be 2 instances of the MuditaOS on the phone (`/sys` is assumed at vfs class creation time).

```bash  
"current"  -> /sys/current  
"previous" -> /sys/previous  
```

The type variable in `boot.json` is for MuditaOS - this will indicate a subdirectory name to append for all file operations (loading assets, dbs, etc.) When the option becomes possible, it should be passed as a variable to the `boot.bin` (MuditaOS) as an argument.

4. `ecoboot` boots the "**boot**" filename in `boot.json`. MuditaOS reads `boot.json` to find it's root directory and reads all assets and files from it. 
   
5. Updating from old style partitioning (1 partition) to new partition scheme (2 partitions). In case of problems see pt 6.

## Creating a storage partition
 
1. Switch the phone to MSC mode (bootloader option 4)

2. Unmount disk from the OS so that it's not used

3. Find the disk name that is assigned to the phone (sda, sdb etc.)

4. Find out if the disk is mounted (assuming /dev/sda)

```bash
[atom@nucleus:~/devel/PurePhone_14/doc]$ cat /etc/mtab | grep /dev/sda | awk '{print $2}'
/mnt
```

5. Unmount `/dev/sda` from `/mnt` before you do anything else

```bash
# sudo umount /mnt 
```

6. Move into the project root directory and run the image generation script

```bash
    ./generate_image.sh pure.img build-rt1051-Debug
```
   
   Example output:
```
Checking that no-one is using this disk right now ... OK

Disk /home/lucck/worksrc/MuditaOS/pure.img: 14,62 GiB, 15678308352 bytes, 30621696 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes

>>> Script header accepted.
>>> Script header accepted.
>>> Script header accepted.
>>> Created a new DOS disklabel with disk identifier 0x09650eb4.
/home/lucck/worksrc/MuditaOS/pure.img1: Created a new partition 1 of type 'W95 FAT32' and of size 1 GiB.
/home/lucck/worksrc/MuditaOS/pure.img2: Created a new partition 2 of type 'W95 FAT32' and of size 1 GiB.
/home/lucck/worksrc/MuditaOS/pure.img3: Created a new partition 3 of type 'Unknown' and of size 12,6 GiB.
/home/lucck/worksrc/MuditaOS/pure.img4: Done.

New situation:
Disklabel type: dos
Disk identifier: 0x09650eb4

Device                                 Boot   Start      End  Sectors  Size Id Type
/home/lucck/worksrc/MuditaOS/pure.img1 *       2048  2099199  2097152    1G  b W95 FAT32
/home/lucck/worksrc/MuditaOS/pure.img2      2099200  4196351  2097152    1G  b W95 FAT32
/home/lucck/worksrc/MuditaOS/pure.img3      4196352 30621695 26425344 12,6G 9e unknown

The partition table has been altered.
Syncing disks.
Littlefs summary:
     Directories created: 5, Files added: 24, Transferred 1198 kbytes.
          Littlefs block size: 32768 blocks: 57/412895.
          /home/lucck/worksrc/MuditaOS

```

7. Once completed, flash the phone with the latest MuditaOS build. To do that run the `pureflash` command 
on the image:

```bash
sudo pureflash pure.img /dev/sdX
```

**Note:** Replace `/dev/sdX` with the disk name you found for your phone after connecting it to your computer.

Example output:

```bash
➜  build-linux-Debug git:(EGD-6188_flashing_script) ✗ ./pureflash ~/worksrc/MuditaOS/build-rt1051-RelWithDebInfo/PurePhone.img /dev/sda 
File /home/lucck/worksrc/MuditaOS/build-rt1051-RelWithDebInfo/PurePhone.img copy 7 extents:
#       Offset          Length           Status
0:      0000000000000000 0000000000001000 OK
1:      0000000000100000 0000000000001000 OK
2:      0000000000104000 0000000000a2d000 OK
3:      0000000040100000 0000000000001000 OK
4:      0000000040104000 0000000000200000 OK
5:      0000000080100000 0000000000010000 OK
6:      0000000320e20000 00000000001c0000 OK
File /home/lucck/worksrc/MuditaOS/build-rt1051-RelWithDebInfo/PurePhone.img verify 7 extents:
#       Offset          Length           Verify
0:      0000000000000000 0000000000001000 OK
1:      0000000000100000 0000000000001000 OK
2:      0000000000104000 0000000000a2d000 OK
3:      0000000040100000 0000000000001000 OK
4:      0000000040104000 0000000000200000 OK
5:      0000000080100000 0000000000010000 OK
6:      0000000320e20000 00000000001c0000 OK
Write image /home/lucck/worksrc/MuditaOS/build-rt1051-RelWithDebInfo/PurePhone.img to /dev/sda SUCCESS
➜  build-linux-Debug git:(EGD-6188_flashing_script) ✗ 
```
