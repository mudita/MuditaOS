PurePhone repository
====================

# Quickstart in docker
You can build project in docker container, to that

### Get docker
`./config/bootstrap.sh 8`

### configure for linux Debug
`./in_docker.sh config linux Debug`

### build linux Debug
`./in_docker.sh make build-linux-Debug`

### build rt1051 Release
`./in_docker.sh config rt1051 Release`
`./in_docker.sh make build-rt1051-Release`

# Quickstart

Prior to any build setup environment, need to be run once. (See: `## Run provisioning`)
`cd config && ./bootstrap.sh`
boostrap.sh will show you list of changes that may be required for you.
If this is new checkout you need to update your git config (step 0 and 1).

### Bootstrap steps:
* `./config/bootstrap.sh 0`  - install style checking scripts to be automatically run on commit
* `./config/bootstrap.sh 1`  - `git blame` will ignore style changing commit
* `./config/bootstrap.sh 2`  - list packages required for builds *but it is not installed*
* `./config/bootstrap.sh 3`  - setup arm toolchain, download and install in home dir
* `./config/bootstrap.sh 4`  - setup cmake, download and install in home dir
* `./config/bootstrap.sh 5`  - list of commands for required for switching default gcc/g++ to version 9
* `./config/bootstrap.sh 6`  - adds Paths for arm toolchain to your PATH environment variable - this is also used by ./env.cmake
* `./config/bootstrap.sh 7`  - adds Paths for cmake to your PATH environment variable
* `./config/bootstrap.sh 8`  - install docker

*6 add_to_path gcc_arm... is required because new ./env.cmake uses environment variables set by this target.*


## style git hooks
During the bootstrap you can install git hooks for style checking.
We have two similar scripts:
pre-commit-check-only.hook  - this hook only notifies you that style is wrong, doesn't change anything
pre-commit.hook             - this hook automatically update style during commit
if you haven't run `bootstrap.sh` you have to copy (link) *one* of this to your 
`.git/config/hooks` directory, just:
`ln -s `pwd`/config/<pre-commit-scipt> .git/hooks/pre-commit`
in the hooks directory script has to be named *pre-commit*.

## Super quick and dirty to run app on linux:
```
git submodule update --init --recursive                                     # initialize submodules
cd ./config/ && ./bootstrap.sh && cd ../                                    # bootstrap requirements
./cofnigure.sh rt1051|linux Debug|Release|RelWithDebInfo                    # configure build
cd <build-dir>                                                              # build dir depends on configuration
make -j                                                                     # build
./PurePhone                                                                 # run PurePhone - emulator screen will pop up
```
## Super quick and dirty to run UT
build the app (as in previous point)
```bash
./configure.sh linux debug
cd <build-dir>
make check
```
## Super quick and dirty to run app on Phone

Prior to all - install JLink driver (see `## Install JLink driver`)

```
cp ./env.cmake.sample ./env.cmake && sed -i "s:<HOME>:$HOME:" env.cmake
git submodule update --init --recursive
cd ./config/ && ./bootstrap.sh && cd ../
./configure.sh rt1051 RelWithDebInfo
cd build-arm-RelWithDebInfo
make -j

```

*now in 3 separate consoles*

Start JLink server
```
./StartJLinkServer.sh
```

Load app with GDB via JLink connected to PurePhone
```
./run.sh <build folder>
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
* need to be run once (not more on restart or anything)

`cd config && ./bootstrap.sh`

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

* [doc/development_workflow](./doc/development_workflow.md)
* [doc/config](./doc/config.linux.md)
* [doc/running_on_phone](./doc/running_on_phone.md)
* [GUI](./module-gui/README.md)
* [doc/bluetooth](./doc/bluetooth.md)
    * [./module-bsp/bsp/bluetooth](./module-bsp/bsp/bluetooth/)
    * [./module-bluetooth/](./module-bluetooth/README.md)
* [module-lwip](./module-lwip/README.md)
* [doc/database](./doc/database_v2.md)
* [Linux emulator keyboard bindings](./doc/host_keyboard_bindings.md)
* [test/harness](./test/README.md)

**Doxygen genertion for GUI:**
`cd doc && doxygen doxyfile-gui`
to show docs:
`firefox docs-gui/html/index.html`
Right now docs are filled for Item.hpp + grouping

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
