# IMPORTANT

Do all the below steps with battery INSERTED in Bell device.
In case of any problems Bell could be hard-reseted by pulling out the battery.

# Prepare Bell

## Burn MCU fuses

Use [D1_eMMC_FlashLoader](https://github.com/mudita/D1_eMMC_FlashLoader/)
to burn MCU fuses on Bell device
and perform eMMC low-level partitioning:

Ozone

1. Run Ozone. 
2. Open FLDebug.jdebug from [D1_eMMC_FlashLoader].
3. Power up the device by connecting USB-C cable
4. Run the program. 
5. USB MSC host should be available. 

MCUXpresso Integrated Development Environment

1. Download and install MCUXpresso Integrated Development Environment (IDE) from www.nxp.com
2. Download, configure and install MCUXpresso Software Development Kit (SDK) from www.nxp.com (choose proper CPU model)
3. Create a new project for [D1_eMMC_FlashLoader] in MCUXpresso
4. Configure to debug with J-LINK
5. Power up the device by connecting USB-C cable

At the end USB MSC should be presented to the host.

## Flash bootloader

Use `master-bell` branch from [ecoboot](https://github.com/mudita/ecoboot/tree/master-bell) to build `ecoboot.bin`. Instructions are provided in repository readme.

You can flash bootloader in two ways:

1. Copy it to Bell filesystem while `D1_eMMC_FlashLoader` is running and umount the device properly. Wait few seconds before stopping `D1_eMMC_FlashLoader` and rebooting the device.

2. Run `ecoboot.bin` with jlink using tools provided in `master-bell` branch of ecoboot repository. Copy `ecoboot.bin` to mounted USB MSC device (to its root folder). Unmount USB MSC device and wait few seconds before rebooting.

Note:
If running on P1 Bell prototype, both switches, placed next to JTAG connector, have to be in the OFF position to run bootloader from board after flashing.

# Run MuditaOS on Bell

Do below steps using [MuditaOS](https://github.com/mudita/MuditaOS)

Use `lsblk` to check Bell device letter (e.g. `/dev/sda`)

## Configure and build Bell image

To configure Bell image, use:

`./configure.sh bell rt1051 RelWithDebInfo -DWITH_BOARD_REVISION=6`

To build Bell image use standard build command, either 'make' or 'ninja'.

## Copy Bell image to Bell device

You can do it in two ways:

1. Use 

`sudo dd if=./BellHybrid.img of=/dev/sdX conv=sparse status=progress`

to copy Bell image to Bell device. `BellHybrid.img` is being build with BellHybrid-disk-img target.

2. Use 

`sudo ./pureflash BellHybrid.img /dev/sdX`


Pureflash tool is placed in build directory. It will download previously built image into /dev/sdX device and verify its contents. 

## Running with JLink

You can run properly build BellHybrid image using jlink and provided tools.
To do that use from root project directory:

1. ./StartJLinkServer.sh T6
2. ./run.sh --product BellHybrid build-bell-rt1051-(buildtype)
