# Running MuditaOS on the Mudita Pure phone

The Mudita Pure phone (RT1051 target platform) has an eMMC storage of 16GB.

Due to RT1051 architecture, the executable can be run from SRAM, DRAM or external SPI flash (eMMC).

Our configuration is always running from [SRAM](#SRAM-(temporary)) either directly (gdb) or by coping a file from [eMMC](#eMMC-(permanent)). 

## Adding assets

No matter which way you want to run the executable, you need to have all the necessary assets.
Everything from `$BUILD_FOLDER/sys/` needs to be [copied onto the eMMC](#eMMC-(permanent)).

Some assets can be omitted though e.g. for fonts the default fallback font will be used.  

## SRAM (temporary)

An executable can be uploaded directly to the memory in an executable (image) form. J-Link GDB Server enables this. J-Link Server messages `"Download 15243 bytes…"` represent this way of running the executable. The binary is lost on reset and/or power loss. Booting from SRAM doesn't involve bootloader.

### Upload to SRAM

You need to upload the `PurePhone.elf` file and then:

- Start J-Link Server with a script `./StartJLinkServer.sh` in a separate terminal/session.
- Run the `./run.sh` script

## eMMC (permanent)

The firmware is copied as a file to a special partition. When running from eMMC, the phone will always boot the image file which means it's permanently "flashed".

Booting from eMMC involves bootloader. There needs to be a working bootloader. The bootloader copies the firmware from the file to SRAM and executes it each time the phone is started.
Files required to boot from eMMC are:
- firmware image `boot.bin`
- firmware image checksum files: `.boot.json` and `.boot.json.crc32`
- assets (images, fonts, sounds, databases)
### Upload to eMMC

- [Mount Mudita Pure as a USB MSC (disk drive)](#How-to-mount-Mudita-Pure-as-a-USB-MSC)
- Use convenience script `flash_eMMC.sh <build_folder; default="build">` to copy both the executable **AND** the assets.

## How to mount Mudita Pure as a USB MSC

There are two ways to mount the phone as a USB MSC - using a bootloader or Ozone debugger.

### Mountin using a bootloader

If you have a bootloader flashed and working use the following key combination, to mount Mudita Pure as MSC:

- hold centre button (selection)
- briefly press reset button (the one closest to the phone on the green breakout board)
- wait while text scrolls from the bottom
- release centre button
- press key 4

### Ozone

If you don't have a bootloader, you can temporarily load a binary that will make the phone appear as a USB MSC. From there you can flash the eMMC as described [above](#Upload-to-eMMC).
