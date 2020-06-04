PurePhone repository
====================

# Quickstart in docker
You can build project in docker container, to that

1. Get docker
`./config/bootstrap.sh 8`

2. configure for linux Debug
`./in_docker.sh config linux Debug`

3. build linux Debug
`./in_docker.sh make build-linux-Debug`

4. build rt1051 Release
`./in_docker.sh config rt1051 Release`
`./in_docker.sh make build-rt1051-Release`

5. build & run tests (linux only)
`./in_docker.sh make build-linux-Debug`
`./in_docker.sh make check`
__do not call `make test` it will make fool of you__

To build manually selected test in work-dir in attached docker image:
`cd ./build-linux-Debug ; make <test_name>; ./<test_name>`
i.e.:
`cd ./build-linux-Debug ; make unittest_gui && ./unittest_gui`

Running tests checks memory leaks too. This might be helpful in creation of new widgets

# Quickstart

Prior to any build setup environment, need to be run once. (See: `## Run provisioning`)
`cd config && ./bootstrap.sh`
boostrap.sh will show you list of changes that may be required for you.
If this is new checkout you need to update your git config (step 0 and 1).

## Bootstrap steps:
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


### Style git hooks
During the bootstrap you can install git hooks for style checking.
pre-commit.hook             - this hook automatically update style during commit
if you haven't run `bootstrap.sh` you have to copy (or link) `pre-commit.hook` to your git conig directory
`.git/config/hooks` directory, just:
`ln -s `pwd`/config/pre-commit.hook .git/hooks/pre-commit`

By default commit hook only checks if your changes have correct style, if you would like to fix
the style automatically during `git commit` you have to configure your git, by adding new
variable `user.fixinstage` and setting it to `true`, just call:
`git config user.fixinstage true`

If you prefer "notification than fix" work flow (so you can examine the changes), use default hook behaviour (to notify)
and then call `./config/pre-commit.hoot --fix`, this checks and fixes files in "stage" are,
files that have status "changed" are not tested.

```
git commit 
<stele error - commit aborted>
./config/pre-commit.hook --fix
git diff
git add -u
git commit
<commit accepted>
```

To fix style for Pull Request CI:
`./config/pre-commit.hook --branch-fix`

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

Catch logs from PurePhone from UART:
1. in `config/ProjectConfig.cmake` change `RTT_JLINK` to `RTT_LUART`
2. rebuild project
3. catch log: `picocom /dev/ttyACM0 -b 115200  --imap lfcrlf`
Please mind that logs on uart are more costly, so these might show unseen before timing issues.
These might slow down phone too - to avoid that consider `release` build for tests with `uart` enabled

# Longstart

There is provisioning script `./config/bootstrap.sh` run it to install all dependencies - written for Ubuntu tested on 18.10
* This script will require sudo (for apt)
* needed cmake and GCC will be installed by default to `${HOME}` - in case of other needs, change it
* script doesn't install ccache, but if ccache is on system - it's support is added to env.cmake.sample
* need to be run once (not more on restart or anything)

`cd config && ./bootstrap.sh`

After running provisioning you are ready to checkout and build project for both linux and rt1051 target:
* checking out project with submodules for the first time
```sh
git submodule update --init --recursive
```
* repo update with submodules
```sh
git pull --recurse-submodules
```
* building project
```sh
./configure.sh [rt1051|linux] [release|debug|relwithdebinfo]
cd build-[rt1051|linux]-[release|debug|relwithdebinfo]
make
```

## Building own docker image

If for some reason you don't want to use existing (on hub.docker.com) docker image you can build your own:
1. download toolchain
```bash
./config/download_assets
```
2. build image
```bash
docker build docker/ -t rwicik/pure_phone_build:latest
```
please make sure that you add proper tag for image (`-t rwicik/pure_phone_build:latest`) as other scripts are using it for building, and if docker couldn't find it locally it will download it from hub.docker.com

## Installing JLink driver

We use JLink driver in version JLink v634f, for ubuntu download from here:
[tested JLink we use](https://www.segger.com/downloads/jlink/JLink_Linux_V634f_x86_64.deb)

To install this driver on linux: `sudo dpkg -i JLink_Linux_V634f_x86_64.deb`

# Documentation

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
* [changelog](./changelog.md) and [changelog howto](./doc/changelog_howto.md)

## Code documentation (doxygen)

You can build code documentation with:
```sh
make doc
```
If you prefer to build documentation always with `all` target instead, configure cmake
with `BUILD_DOC_WITH_ALL` option set, e.g.:
```sh
cmake -DBUILD_DOC_WITH_ALL=ON . build/
```

Documentation is generated in the `doc/html` subdirectory of a build dir. To view
open index with browser, e.g.:
```sh
firefox build/doc/html/index.html
```

## Miscellaneous CMake configuration options

Following CMake options can be used:
 * `COLOR_OUTPUT` - enable/disable colored output (enabled by default). Useful when you need to process CMake output or your IDE does not support escape codes.
 * `BUILD_DOC_WITH_ALL` - enable disable building code documentation with the `all` target (disabled by default).

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
