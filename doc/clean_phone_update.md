To start you need to obtain a working version of the PureTesterPCApp.

The repository for this application is at: https://github.com/mudita/PureTesterPCApp
it's qt based so to build it you'll need a couple of extra packages (names for debian
bases linux distro):

`# apt-get install qt5-qmake libqt5serialport5-dev libudev-dev`

clone the repo and build it using qmake and make.

`# git clone git@github.com:mudita/PureTesterPCApp.git`

this application is maintained byt the Electronics team of Mudita and they are
responsible for this repo in case on any errors please consult them for help.

By default this app looks for a binary file to load to memory of the phone, the
path it uses is ../../PureHardwareTester/build/PureHardwareTester.bin relative to
the binary. You can either make those directories and use them or recompile the program
with a different path (connecttab.cpp line #164)

(below we assume default compile with DEVELOPER_CASE enabled)
Create the build directory:

`# mkdir -p ../../PureHardwareTester/build`

Download a copy of ecoboot.bin from github
https://github.com/mudita/ecoboot/releases/tag/1.0.4

replace the file PureHardwareTester.bin as defined in the PureTesterPCApp
with ecoboot.bin

`# cp ecoboot.bin ../../PureHardwareTester/build/PureHardwareTester.bin`

Now run the app as root (this is important running it as a normal user might
fail if you are not in proper groups)

`# sudo ./PureTester`

You should see a screen like this:
![](./Images/pure_hardware_tester_start.png "")

Now hold the middle joystick button on the phone (as usual when you want to
enter the bootloader) and click connect on the app, you should ge a normal
bootloader screen on the phone and an output from the app like this:
![](./Images/pure_hardware_tester_connected.png "")
