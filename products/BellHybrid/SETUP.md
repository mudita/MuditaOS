# IMPORTANT

Do all the below steps with battery INSERTED in Bell device.
In case of any problems, you can hard-reset Bell by pulling out the battery.

# Prepare Bell

## Burn MCU fuses

Use [D1_eMMC_FlashLoader](https://github.com/mudita/D1_eMMC_FlashLoader/)
to burn MCU fuses on Bell device
and perform eMMC low-level partitioning:

Ozone

1. Run Ozone.
2. Open FLDebug.jdebug from [D1_eMMC_FlashLoader].
3. Power up the device by connecting the USB-C cable
4. Run the program.
5. USB MSC host should be available.

MCUXpresso Integrated Development Environment

1. Download and install MCUXpresso Integrated Development Environment (IDE) from www.nxp.com
2. Download, configure and install MCUXpresso Software Development Kit (SDK) from www.nxp.com (choose proper CPU model)
3. Create a new project for [D1_eMMC_FlashLoader] in MCUXpresso
4. Configure to debug with J-Link
5. Power up the device by connecting the USB-C cable

## Create FAT partition

In case of flashing the device for the first time, you need to format the storage to FAT32 in order for the desktop OS to detect it.
On Linux, using GParted:
1. Select correct USB MSC device from a device pull-up menu
2. Choose Device > *Create Partition Table...* and create a *MS-DOS* partition table
3. Add a partition and format it to FAT32
4. Apply changes and restart device

On macOS, using Disk Utility:
1. Choose View > *Show All Devices*
2. In the sidebar, select the disk you want to format.
3. Click the Erase button in the toolbar.
4. In the Erase dialog, click the Scheme pop-up menu, then choose *Master Boot Record* partition scheme.
5. Click the Format pop-up menu, then choose *MS-DOS (FAT)*
6. (Optionally) Enter a name for the volume.
7. Click Erase, then click Done.

When reconnected to PC, USB MSC should be presented to the host.

## Flash bootloader

Use the `master-bell` branch from [ecoboot](https://github.com/mudita/ecoboot/tree/master-bell) to build `ecoboot.bin`. Instructions are provided in the repository's [README.md](https://github.com/mudita/ecoboot/blob/master-bell/README.md).

You can flash bootloader in two ways:

1. Copy it to the Bell filesystem while `D1_eMMC_FlashLoader` is running and unmount the device properly. Wait a few seconds before stopping `D1_eMMC_FlashLoader` and rebooting the device.

2. Run `ecoboot.bin` with J-Link using tools provided on the `master-bell` branch of ecoboot repository. Copy `ecoboot.bin` to the mounted USB MSC device (to its root folder). Unmount USB MSC device and wait a few seconds before rebooting.

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

## Running with J-Link

You can run properly build BellHybrid image using J-Link and provided tools.
To do that use the script from the root project directory:

1. ./StartJLinkServer.sh T6
2. ./run.sh --product BellHybrid build-bell-rt1051-(buildtype)
