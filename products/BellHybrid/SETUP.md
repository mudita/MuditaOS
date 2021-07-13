# IMPORTANT

Do all the below steps with battery INSERTED in Bell device.

In case of any problems Bell could be hard-reseted by pulling out the battery.

# Prepare Bell

## Burn MCU fuses

Use [D1_eMMC_FlashLoader](https://github.com/mudita/D1_eMMC_FlashLoader/)
to burn MCU fuses on Bell device.

## Flash bootloader

Use `master-bell` branch from [ecoboot](https://github.com/mudita/ecoboot/tree/master-bell)
to create `ecoboot.bin` and copy it to Bell filesystem
while `D1_eMMC_FlashLoader` is running.

# Run MuditaOS on Bell

Do below steps using [MuditaOS](https://github.com/mudita/MuditaOS)

Use `lsblk` to check Bell device letter (e.g. `/dev/sda`)

## Partition eMMC

Use

`sudo ./config/partition_emmc.sh /dev/sdX -f BellHybrid`

to partition Bell device.

## Copy Bell image to Bell device

Use (`BellHybrid.img` will be in build folder)

`sudo dd if=./BellHybrid.img of=/dev/sdX conv=sparse status=progress`

to copy Bell image to Bell device
