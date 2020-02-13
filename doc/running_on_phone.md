# Flashing phone

The phone (rt1051 target platform) has eMMC storage of 16GB.

Due to rt1051 arch, the executable can be run from SRAM, DRAM or external SPI flash (eMMC).

Our configuration is always running from [SRAM](#SRAM) either directly (gdb) or coping a file from [eMMC](#eMMC). 

## assets  
No matter which way you want to run the executable, there needs to be assets aka. resources.
Everything from `$BUILD_FOLDER/sys/` needs to be [copied onto the eMMC](#eMMC_upload).

Some assets can be missing though e.g. for fonts the default fallback font will be used instead.  

## SRAM (temporary) <a name="SRAM"></a>
An executable can be uploaded directly to the memory in a executable (image) form.
JLinkServer + gdb combo enables this.
JLinkServer messages "Download 15243 bytes…" represents this way of running the executable.
The binary is lost on reset / power loss. 
Booting from RAM doesn't involve bootloader.

### upload to SRAM
file to be uploaded: `PurePhone.elf`
- Start JLinkServer with a script `./StartJLinkServer.sh` in a separate terminal.
- Run `./run.sh` 

---

## eMMC (permanent) <a name="eMMC"></a>
The firmware is copied as a file to a special partition.
When running from eMMC, the phone will always boot the image file = it is permanently "flashed'.

Booting from eMMC involves bootloader. There needs to be working bootloader. The bootloader copies the firmware from the file to RAM and executes it.

### <a name="eMMC_upload">upload to eMMC</a>
file to be copied: `boot.bin`
- [mount PurePhone as USB MSC (disk drive)](#mount)
- use convenience script `flash_eMMC.sh <build_folder; default="build">` to copy both executable **AND assets**

***

## how to mount PurePhone as USB MSC <a name="mount"></a>
There are two ways to mount the phone as USB MSC:
### Bootloader
If you have bootloader flashed and working use a key combo, to mount PurePhone as MSC:
- hold centre button (selection)
- briefly press reset button (the one closest to the phone on the green breakout board)
- wait while text scrolls from the bottom
- release centre button
- press key 4

### Ozone
If you don't have bootloader, you can temporarily load a binary that will make the phone appear as USB MSC.
From there you can flash the eMMC as described [above](#eMMC_upload).


Details can be found here: [D1_eMMC_FlashLoader](https://github.com/muditacom/D1_eMMC_FlashLoader) 
