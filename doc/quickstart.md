# Quickstart

You can quickstart the project by going through one of the following guides:

- [Quickstart using Docker](#)
- [Building your own Docker image](#)
- [Quickstart in your local environment](#)
- [Longstart](#)
- [Super dirty quickstart on Linux](#)
- [Super dirty quickstart with unit tests](#)
- [Super dirty quickstart on the phone](#)


## Quickstart using Docker
You can build MuditaOS using a Docker container. To do that, follow these steps

1. Get Docker by running `./config/bootstrap.sh 8` or [install it yourself](https://www.docker.com/get-started).

2. Add yourself to a `docker` group:

```bash
NAME=$(whoami)
sudo usermod -aG docker ${NAME}
```
After that you have to log out and log back in as groups are set during the login process. To make sure you are in docker group enter the `groups` command and 
you will see the list of groups your user is assigned to.

3. Configure for Linux Debug: `./in_docker.sh config linux Debug`

4. Build linux Debug: `./in_docker.sh make build-linux-Debug`

5. Build RT1051 Release

`./in_docker.sh config rt1051 Release`
`./in_docker.sh make build-rt1051-Release`

6. Build and run tests (Linux only)

`./in_docker.sh make build-linux-Debug`
`./in_docker.sh make check`

To build selected test manually in the working directory of attached Docker image run:
`cd ./build-linux-Debug ; make <test_name>; ./<test_name>`
i.e.:
`cd ./build-linux-Debug ; make unittest_gui && ./unittest_gui`

Running tests checks memory leaks too - this might be helpful in creation of new widgets.

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

## Quickstart in your local environment

Prior to setting up your environment, please run the following command: `cd config && ./bootstrap.sh`

A provisioning script named `boostrap.sh` will go through installation steps described below and show you a list of changes that may be required for you to set up the environment locally.

If the script is run after a fresh `git clone` you need to update your git config (step 0 and 1).

### `bootstrap.sh` steps:
* `./config/bootstrap.sh 0`  - install style checking scripts to be automatically run on commit
* `./config/bootstrap.sh 1`  - `git blame` will ignore style changing commit
* `./config/bootstrap.sh 2`  - list packages required for setting up the environment *which are currently not installed*
* `./config/bootstrap.sh 3`  - set up arm toolchain, download and install it in home directory
* `./config/bootstrap.sh 4`  - set up `cmake`, download and install in home directory
* `./config/bootstrap.sh 5`  - list of commands required for switching default gcc/g++ to version 9
* `./config/bootstrap.sh 6`  - add paths for arm toolchain to your `PATH` environment variable - this is also used by `./env.cmake` *(this is required because `./env.cmake` uses environment variables set by this target)*
* `./config/bootstrap.sh 7`  - add paths for `cmake` to your `PATH` environment variable
* `./config/bootstrap.sh 8`  - install Docker

#### Code style with git hooks

The `bootstrap.sh` script installs git hooks for code style checking. `pre-commit.hook`automatically updates style during commit. If you haven't run `bootstrap.sh` and want to use git hooks, you have to copy (or link) `pre-commit.hook` to your git config directory `.git/config/hooks`: `ln -s `pwd`/config/pre-commit.hook .git/hooks/pre-commit`

By default commit hook only checks if your changes have the appropriate style, if you would like to fix the style automatically during `git commit` you have to configure your git, by adding new variable `user.fixinstage` and setting it to `true` by running
`git config user.fixinstage true`

If you prefer "notification than fix" workflow (so you can examine the changes), use the default hook behaviour (for notifications) and then call `./config/pre-commit.hook --fix`, this checks and fixes files in "stage", files that have status "changed" are not tested.

```
git commit 
<stele error - commit aborted>
./config/pre-commit.hook --fix
git diff
git add -u
git commit
<commit accepted>
```

To fix the style for Pull Request CI:
`./config/pre-commit.hook --branch-fix`

## Longstart

Run the provisioning script `./config/bootstrap.sh` to install all dependencies - written for Ubuntu, tested on 18.10

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

## Super dirty quickstart on Linux

Here's the bare minimum that will enable you to bootstrap the environment on Linux.

```
git submodule update --init --recursive                        # initialize submodules
cd ./config/ && ./bootstrap.sh && cd ../                       # bootstrap requirements
./cofnigure.sh rt1051|linux Debug|Release|RelWithDebInfo       # configure build
cd <build-dir>                                                 # build dir depends on configuration
make -j                                                        # build
./PurePhone                                                    # run PurePhone - emulator screen will pop up
```
## Super dirty quickstart with unit tests

After going through the Super dirty quickstart on Linux, run the following commands:

```bash
./configure.sh linux debug
cd <build-dir>
make check
```
## Super dirty quickstart on the phone

1. First install J-Link driver. We suggest using J-Link driver in version J-Link v634f ([Ubuntu download](https://www.segger.com/downloads/jlink/JLink_Linux_V634f_x86_64.deb)

2. Please run the following commands:

```
cp ./env.cmake.sample ./env.cmake && sed -i "s:<HOME>:$HOME:" env.cmake
git submodule update --init --recursive
cd ./config/ && ./bootstrap.sh && cd ../
./configure.sh rt1051 RelWithDebInfo
cd build-arm-RelWithDebInfo
make -j

```

3. Please take the following steps in three parallel consoles/sessions:

 - Start J-Link server: `./StartJLinkServer.sh`
 - Load app with GDB via J-Link connected to Mudita Pure: `./run.sh <build folder>`
 - Catch logs from Mudita Pure from J-Link RTT and write them to `/tmp/log.txt`: `JLinkRTTClient | tee /tmp/log.txt`


### Catching logs using UART

If you want to catch logs from Mudita Pure from UART please follow these steps:

1. In `config/ProjectConfig.cmake` change `RTT_JLINK` to `RTT_LUART`
2. Rebuild the project
3. Catch logs: `picocom /dev/ttyACM0 -b 115200  --imap lfcrlf`

Please mind that logs on UART are more costly, so these might cause timing issues and slow down the phone too. To avoid that consider `release` build for tests with `uart` enabled.





## Preparing Packages

If you need a package, containing everything needed to run application (on target device or on linux)
in the build directory call
```bash
make package
```
on finish command will display the name of the package that was created.

Package name is: *PurePhone-<version>-<target>.<extension>*
where:
    <version>   - is readed from the latest "release-x.y.z" tag
    <target>    - RT1051 or Linux 
    <extension> - `zip` for RT1051 and `tar.gz` for Linux
