# Running MuditaOS on the Mudita Pure phone

The Mudita Pure phone (RT1051 target platform) has an eMMC storage of 16GB.

Due to RT1051 architecture, the executable can be run from SDRAM, DRAM or external flash (eMMC).

## Adding assets

No matter which way you want to run the executable, you need to have all the necessary assets.
Everything from `$BUILD_FOLDER/sys/` needs to be [copied onto the eMMC](#emmc-permanent).

Some assets can be omitted though e.g. for fonts the default fallback font will be used. 

## SDRAM (temporary)

An executable can be uploaded directly to the memory in an executable (image) form. J-Link GDB Server enables this. J-Link Server messages `"Download 15243 bytes…"` represent this way of running the executable. The binary is lost on reset and/or power loss. Booting from SDRAM doesn't involve bootloader.

### Upload to SDRAM via J-Link

You need to upload the `PurePhone.elf` file. To do so run in three parallel consoles/sessions:

- Start J-Link server:
    - For Mudita Pure: `./StartJLinkServer.sh`
    - For BellHybrid:  `./StartJLinkServer.sh T6`
- Load app with GDB via J-Link connected:
    - to Mudita Pure: `./run.sh <build folder> --product PurePhone`
    - to BellHybrid:  `./run.sh <build folder> --product BellHybrid`
- Catch logs from Mudita Pure from J-Link RTT and write them to `/tmp/log.txt`: `JLinkRTTClient | tee /tmp/log.txt`

**NOTE:** Running on phone is done via gdb, this way enables you to debug running target.

## eMMC (permanent)

The firmware is copied as a file to a special partition. When running from eMMC, the phone will always boot the image file which means it's permanently "flashed".

Booting from eMMC involves bootloader. There needs to be a working bootloader. The bootloader copies the firmware from the file to SDRAM and executes it each time the phone is started.

Files required to boot from eMMC are:
- firmware image `boot.bin`
- firmware image checksum files: `.boot.json` and `.boot.json.crc32`
- assets (images, fonts, sounds, databases)

### Uploading it to the eMMC

####  via bootloader

On linux you can use:
1. `pureflash`

The fastest command to burn image into the eMMC card:  `sudo pureflash <PurePhone.img> /dev/sdX`
**Note:** Replace `/dev/sdX` with the disk name you found for your phone after connecting it to your computer.

2. `dd` or `etcher`

You can use any raw memory copy tool such as `dd` or `etcher` which will transfer the whole image built to the device memory. It will take much longer because it will copy the whole 16GB of memory.
dd example:
```
sudo dd if=./PurePhone.img of=/dev/sdb bs=1M status=progress conv=fsync                                                                                                                                                                                                    13:48:22
```

On MacOS you can use script:
- `./tools/macflash.sh`

**NOTE:** End-user devices have direct access to the eMMC locked in bootloader.

#### via updater utility

Every device is loaded with additional updater software which is capable of updating any software package onto the device while keeping the last software as a backup.
Updater utility is the only normal way to update devices running in-field and is used by Mudita Center.

**NOTE:** Providing a minimum viable update package and utility for the Open Source community is on our backlog. It can be created by trimming additional development API from:
[updater feature tests](https://github.com/mudita/QAMuditaOS/tree/master/scenarios/updater). You can ask for the development version of Mudita Center with "update from file" capability enabled.

Update package is described here: [build targets](build_targets.md)

## How to mount Mudita Pure as a USB MSC

**NOTE:** all phones in field have eMMC access locked with a password, the following documentation is only for development devices or ones with eMMC unlocked or bypassed.

There are two ways to mount the phone as a USB MSC to gain full access to the memory - using a bootloader or Ozone debugger.

### Mounting using a bootloader

**NOTE:** Devices in field have password-locked bootloaders. This is by design - normal users shouldn't ever require access to raw eMMC.

- hold center button (selection)
- reset the device, either:
    - briefly press reset button (the one closest to the phone on the green breakout board)
    - detach USB and battery, then attach battery then USB
- wait while text scrolls from the bottom - still holding the center button
- release centre button
- depending on bootloader:
    - with eMMC locked:
        - press 9
        - input passkey
        - confirm with left functional
    - with eMMC not locked:
        - press key 4

We can have 3 types of partitions:
- FAT
- EXT4
- LittleFS

Both FAT and ext4 can be mounted natively on Linux, ext4 needs additional drivers on macOS.   To access LittleFS you need to mount it via FUSE using the following command `lfsfuse /dev/sdX <mount_point> --block_size=32768`
