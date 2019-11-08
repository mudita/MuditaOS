PurePhone repository
====================

# Quickstart

## Super quick and dirty to run app on linux:
```
cp ./env.cmake.sample ./env.cmake && sed -i "s:<HOME>:$HOME:" env.cmake     # set env.cmake
git submodule update --init --recursive                                     # initialize submodules
cd ./config/ && ./bootstrap.sh && cd ../                                    # bootstrap requirements
./rebuild.sh && make -C build -j4                                           # run rebuild (cmake configuration) and build
cd build && ./PurePhone                                                     # run PurePhone - emulator screen will pop up
```

## Super quick and dirty to run app on Phone

Prior to all - install JLink driver (see `## Install JLink driver`)

```
cp ./env.cmake.sample ./env.cmake && sed -i "s:<HOME>:$HOME:" env.cmake
git submodule update --init --recursive
cd ./config/ && ./bootstrap.sh && cd ../
./rebuild.sh rt1051 && make -C build -j4
```

*now in 3 separate consoles*

Start JLink server
```
./StartJLinkServer.sh
```

Load app with GDB via JLink connected to PurePhone
```
./run.sh
```

Catch logs from PurePhone from JLink RTT and write them to /tmp/log.txt
```
JLinkRTTClient | tee /tmp/log.txt
```

# Longstart

## Run provisioning

There is provisioning script `./config/bootstrap.sh` run it to install all dependencies - written for Ubuntu tested on 18.10
* This script will require sudo (for apt)
* needed cmake and GCC will be installed by default to `${HOME}` - in case of other needs, change it
* script doesn't install ccache, but if ccache is on system - it's support is added to env.cmake.sample

## Install JLink driver:

We use JLink driver in version JLink v634f, for ubuntu download from here:
[tested JLink we use](https://www.segger.com/downloads/jlink/JLink_Linux_V634f_x86_64.deb)

To install this driver on linux:
`sudo dpkg -i JLink_Linux_V634f_x86_64.deb`

## First time repo checkout with submodules
`git submodule update --init --recursive`

## Building project

after running provisioning you are ready to build project for both linux and rt1051 target

## Repo update with submodules
`git pull --recurse-submodules`

# Documentation:

* [doc/config](./doc/config.linux.md)
* [doc/bluetooth](./doc/bluetooth.md)
    * [./module-bsp/bsp/bluetooth](./module-bsp/bsp/bluetooth/)
    * [./module-bluetooth/](./module-bluetooth/README.md)
* [module-lwip](./module-lwip/README.md)
* [doc/database](./doc/database_v2.md)
* [emulator keyboard bindings](./doc/host_keyboard_bindings.md)

# Linux Bluetooth device

Bluetooth service by default initializes bluetooth device. 
On linux it's tellit developer kit plugged via USB-CDC. To properly
inteface with bluetooth telit device (and not have conflicts in names)
please add following script as: `/etc/udev/rules.d/99-purephone.rules` on linux.

```
ACTION=="add", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="AL02ZDVH", GROUP="dialout", SYMLINK+="telit"
ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE="666", GROUP="dialout"
```

This will enable you to use Bluetooth telit board as `/dev/telit` which 
is expected by code.
To access devices without root priviledges add yourselve to dialout group.
`sudo usermod -a -G dialout $USER`

For changes to take place run:
`sudo udevadm control --reload-rules && sudo udevadm trigger`
