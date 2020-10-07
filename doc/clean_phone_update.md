# How to install MuditaOS bootloader on a fresh Mudita Pure

To start you need to obtain a working version of the [PureTesterPCApp](https://github.com/mudita/PureTesterPCApp).

The application is Qt-based so to build it you'll need a couple of extra packages (names for Debian-based Linux distros):

1. Start with: `# apt-get install qt5-qmake libqt5serialport5-dev libudev-dev`

2. Clone the [PureTesterPCApp repository](https://github.com/mudita/PureTesterPCApp) and build it using `qmake` and `make`.

`# git clone git@github.com:mudita/PureTesterPCApp.git` 

This application is maintained by Mudita Electronics team and they are
responsible for this repo in case on any errors please consult them for help. 

By default this app looks for a binary file to load to memory of the phone, the
path it uses is `../../PureHardwareTester/build/PureHardwareTester.bin` relative to the binary. You can either make those directories and use them or recompile the program with a different path (`connecttab.cpp` line #164). In the below steps we assume default compile with DEVELOPER_CASE enabled.

3. Create the build directory:

`# mkdir -p ../../PureHardwareTester/build`

4. Download a copy of `ecoboot.bin` from [GitHub ecoboot repository](https://github.com/mudita/ecoboot/releases/tag/1.0.4).

5. Replace the file `PureHardwareTester.bin` as defined in the PureTesterPCApp
with `ecoboot.bin`

`# cp ecoboot.bin ../../PureHardwareTester/build/PureHardwareTester.bin`

6. Now run the app as root (this is important running it as a normal user might
fail if you are not in proper groups)

`# sudo ./PureTester`

You should see a screen like this:
![](./Images/pure_hardware_tester_start.png "")

7. Now hold the middle joystick button on the Mudita Pure phone (this will enter the bootloader) and click connect on the app, you should get a normal
bootloader screen on the phone and an output from the app like this:

![](./Images/pure_hardware_tester_connected.png "")

8. After this press `4` on the phone and it should enter the normal USB-MSC mode.

If the phone is clean it will only report one disk with no partitions, if that's the case you should follow point # from ["How to boot Mudita Pure and create a storage partition" article](https://github.com/mudita/PurePhone/blob/master/doc/boot_and_update.md) - this will tell you how to flash the phone with new software.

9. You also need to flash some version of `ecoboot.bin`. To do that after flashing the phone repeat this procedure and enter `ecoboot` in USB-MSC mode once again, now copy `ecoboot.bin` to the root of the first disk named **PUREOS** and safely remove the disk from the OS using your file manager, you can also do it using command line (assuming `/dev/sda` is the PURE usb disk)

`# udisksctl power-off -b /dev/sda`

This should start the ecoboot ubdate process.

If one of the above steps fails, you can see what's going on by connecting to the serial port of the J-Link device (`/dev/ttyAMA*``), all information from the bootloader when it runs and is trying to update itself are logged there.

## If all of the above fails

If the above method does not work, you will have to use J-Link and the Ozone
debugger. 

1. Connect the J-Link device to the phone and download the needed
drivers and Ozone. Clone the [D1 FLash loader repository](https://github.com/mudita/D1_eMMC_FlashLoader) to be used in Ozone

2. [Download the binary release for D1](https://github.com/mudita/D1_eMMC_FlashLoader/releases) from the releases page of the project. With the binary you don't need to rebuild the project (download the `.axf` file)

3. In the cloned project directory create a directory named `Debug` and place the `.axf` file in it:

`# mkdir Debug`

`# cp D1_eMMC_FlashLoader.axf Debug/`

4. Open the project in Ozone using the `FLDebug.jdebug` file, then connect do the device and run the code. The phone should switch to USB storage mode and you can now access the disk. It also updates the bootloader code the same way as `ecoboot` does, just place the current `ecoboot.bin` file in the root directory of the first partition and eject the disk using your file manager or using cli (assuming your phones is `/dev/sda`):

`# udisksctl power-off -b /dev/sda`
