# IMPORTANT

Do all the below steps with battery INSERTED in Bell device.

In case of any problems Bell could be hard-reseted by pulling out the battery.

# Prepare Bell

## Burn MCU fuses

Use [D1_eMMC_FlashLoader](https://github.com/mudita/D1_eMMC_FlashLoader/)
to burn MCU fuses on Bell device
and perform eMMC low-level partitioning:

1. Download and install MCUXpresso Integrated Development Environment (IDE) from www.nxp.com
2. Download, configure and install MCUXpresso Software Development Kit (SDK) from www.nxp.com (choose proper CPU model)
3. Create a new project for [D1_eMMC_FlashLoader] in MCUXpresso
4. Configure to debug with J-LINK
5. Power up the device by connecting USB-C cable

At the end USB MSC should be presented to the host.

## Flash bootloader

Use `master-bell` branch from [ecoboot](https://github.com/mudita/ecoboot/tree/master-bell)
to create `ecoboot.bin` and copy it to Bell filesystem
while `D1_eMMC_FlashLoader` is running.

# Run MuditaOS on Bell

Do below steps using [MuditaOS](https://github.com/mudita/MuditaOS)

Use `lsblk` to check Bell device letter (e.g. `/dev/sda`)

## Partition eMMC

Use

`sudo ./config/partition_emmc.sh /dev/sdX -f -p BellHybrid`

to partition Bell device.

## Copy Bell image to Bell device

Use (`BellHybrid.img` will be in build folder)

`sudo dd if=./BellHybrid.img of=/dev/sdX conv=sparse status=progress`

to copy Bell image to Bell device
