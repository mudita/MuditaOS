# Development environment setup

Table of Contents
=================

* [Development environment setup](#development-environment-setup)
   * [Introduction](#introduction)
      * [Download repository and submodules](#download-repository-and-submodules)
      * [Install dependencies](#install-dependencies)
         * [bootstrap](#bootstrap)
         * [JLink software](#jlink-software)
         * [Add GitHub token](#add-github-token)
         * [Project configuration](#project-configuration)
         * [Project build](#project-build)
         * [Project load and run](#project-load-and-run)
            * [Running on linux](#running-on-linux)
            * [Running on rt1051 from emmc](#running-on-rt1051-from-emmc)
      * [checking commits](#checking-commits)
      * [CMake options](#cmake-options)
         * [Catching logs using UART](#catching-logs-using-uart)
   * [More details on preparing your local environment](#more-details-on-preparing-your-local-environment)
      * [Code style with git hooks](#code-style-with-git-hooks)
      * [Commit message template](#commit-message-template)
      * [Commit message hook](#commit-message-hook)
         * [Adding a hook:](#adding-a-hook)
         * [Using a hook:](#using-a-hook)
   * [Build using Docker](#build-using-docker)
      * [Building your own Docker image](#building-your-own-docker-image)
   * [Preparing packages](#preparing-packages)

## Introduction

The project can be build:
- on linux
- on windows with [WSL](https://docs.microsoft.com/en-us/windows/wsl/)
- with docker image
Note: Mac native compilation **is not supported**

### Download repository and submodules

MuditaOS uses submodules, therefore for clone use:
```
git clone --recurse-submodules git@github.com:mudita/MuditaOS.git
```
if you didn't use `--recurse-submodules` or have to update submodules use:
```
git submodule update --init --recursive
```

### Install dependencies

#### bootstrap

To install all the basic depenencies run: 
```
cd config && ./bootstrap.sh 0-
```

The script is written for Ubuntu and tested on 20.04.  

**Note** The script:
-requires sudo to:
    - downloads a shell script from `get.docker.com` and runs it with sudo for Docker installation.
    - installs a few required packages (list them with: `cat config/bootstrap_config`) which also require root privileges.
- installs `GCCv10` and `CMake` to `$HOME` directory.
- it's needed to be run only once

After running provisioning you are ready to checkout and build project for both platforms supported.

You can see this `help` if you do not pass any param to `bootstrap.sh`:
```
[I] ⋊> ~/w/m/M/config on master ⨯ ./bootstrap.sh                                                                                                                                                                                                                                    09:00:01
test_if_run_as_root
Available build steps:
	0 install_hooks
	1 add_ignore_revs_for_blame
	2 install_ubuntu_packages
	3 setup_arm_toolchain
	4 setup_cmake
	5 setup_gcc_alternatives
	6 add_to_path ARM_GCC /home/pholat/gcc-arm-none-eabi-10-2020-q4-major/bin
	7 add_to_path CMAKE /home/pholat/cmake-3.21.3-linux-x86_64/bin
	8 install_docker
	9 add_to_docker_group
call:
./bootstrap.sh <build step no>[-]
ex.:
./bootstrap.sh 1       - build step 1 (add_ignore_revs_for_blame)
./bootstrap.sh 3       - build step 3 (setup_arm_toolchain)
./bootstrap.sh 3-      - build from step 3 to the end
./bootstrap.sh 0-      - build everything
```

#### JLink software


We are using J-Link driver in version J-Link v634f ([Ubuntu download](https://www.segger.com/downloads/jlink/JLink_Linux_V634f_x86_64.deb))  

**NOTE:** This step is only required to load software and debug it via J-Link. It's not available for the community - as it requires a programmer board and soldered in programmer socket.

**WARNING:** newer J-Link software revisions seem to work fine, but loading via them does not result in usable software on the hardware target.

#### Add GitHub token

Please follow github token configuration here: [download assets documentation](../doc/download_asset.py)

#### Project configuration

To configure project we have helper script: `./configure.sh` which essentially passes through basic cmake configuration for each product for each platform.
We highly advise using `ninja` as it has proven better compilation times over make.
To build MuditaOS with no access to proprietary files, please specify cmake flag: `-DASSETS_TYPE=Community`

The script can be run with the following parameters:
```
# command      # product              # platform     # build type
./configure.sh [PurePhone|BellHybrid] [rt1051|linux] [release|debug|relwithdebinfo] [additional cmake parameters and options go here, i.e. -DENABLE_DEVELOPER_MODE_ENDPOINT=1 or "-G Ninja"]
```

__Examples:__

1. building PurePhone for rt1051 platform with ninja

**NOTE** Due to software size we can't have full debug builds for rt1051 targets. Please use relwithdebinfo

```
./configure.sh pure rt1051 relWithDebInfo "-G Ninja"
```

2. building PurePhone for Linux platform with make

```
./configure.sh pure linux debug
```

3. building bell for linux platform with ninja

```
./configure.sh bell linux debug "-G Ninja"
```

4. building bell for linux platform with ninja and only community assets

```
./configure.sh bell linux debug -DASSETS_TYPE=Community "-G Ninja"
```

#### Project build

Each run of `configure.sh` creates `build-{PRODUCT}-{PLATFORM}-{OPTMALIZAION}` folder, i.e.: `build-PurePhone-rt1051-RelWithDebInfo`
1. enter your selected build catalog
2. run `ninja` or `make` depending on your selection during configuration.
3. This will compile your selected target and create image of it

To know more about build targets please see: [build targets documentation](../doc/build_targets.md)

**WARNING:** our source code is open source, but the project itself is in the progress of fully embracing the community. Currently, you:
1. can build binaries from the software
2. are not able to create images - due to difficulties with separation of 3rd party proprietary assets

**Note** You can install and use ccache to speed up compilations

#### Project load and run

Project load and run are platform-dependent. Currently, we support only Linux and rt1051 targets.

**NOTE:** Addition of open source assets is in progress and should be done in the following weeks. Till then Open source community can't build their own images - due to a lack of fonts and LUTs. Support will be added step by step:
- addition of open source fonts
- addition of partial update packages (not signed) for Open Source contributors

##### Running on linux

Build default target and run `./PurePhone.elf` for phone target, or for BellHybrid: `./BellHybrid.elf`  

##### Running on rt1051 from emmc

Please follow: [running on rt1051 platform](./running_on_phone.md)

### checking commits

All commits have to comply with checks with:
1. our pre commit hook
    1.1 required code have to be copyrighted with Mudita licensing
    1.2 required code have to comply to clang-format defined in repository
    1.3 code is checked against addition of binary blobs polluting software
2. pre-merge CI job checking if the commit is in acceptable format:

See [development workflow](./development_workflow.md) for more information

### CMake options
Before configuring the project with `./configure.sh` you can tune it by enabling/disabling options to suite your needs.
This can be done manually, by editing the `.cmake` files (not recommended though) or by CLI/GUI tool like `ccmake`.

| Option                        | Description                                                               | Default value |
| ------------------------------|:-------------------------------------------------------------------------:|:-------------:|
| `COVERAGE_ENABLE`             | Enable code coverage report generation                                    | OFF           |
| `COLOR_OUTPUT`                | Use colored output in RTT logs and compiler diagnostics                   | ON            |
| `SYSTEMVIEW`                  | Enable usage of Segger's SystemView                                       | OFF           |
| `USBCDC_ECHO`                 | Enable echoing through USB-CDC                                            | OFF           |
| `MUDITA_USB_ID`               | Enable using Mudita registered USB Vendor ID and Pure Phone USB Product ID| OFF           |
| `ENABLE_APP_X`                | Build and enable application X                                            | ON            |
| `OPTIMIZE_APP_X`              | Optimize application X in debug build                                     | ON            |
| `LINUX_ENABLE_SANITIZER`      | Enable address sanitizer for Linux                                        | ON            |
| `ENABLE_SECURE_BOOT`          | Build signed binary for Secure Boot                                       | OFF           |
| `THIRD_PARTY_DEBUG_OPTIMIZE`  | Optimize third party in debug                                             | ON            |
| `ENABLE_TEST_LOGS`            | Enable logs in unit tests                                                 | OFF           |
| `GENERATE_STACK_USAGE`        | Generate stack usage report                                               | OFF           |
| `BUILD_DOC_WITH_ALL`          | Build documentation with `all` target                                     | OFF           |
| `SYSTEM_PROFILE`              | Add MuditaOS x FreeRTOS proifling capability                              | OFF           |
| `WITH_DEVELOPMENT_FEATURES`   | Enable all development features like access to test harness via USB       | OFF           |

By using `ENABLE_APP_X` (where `X` is the name of the application) you can enable/disable any application.

CMake-wide system config documentation is here: [Project config](ProjectConfig.md)

#### Catching logs using UART

If you want to catch logs from Mudita Pure from UART please follow these steps:

1. In `config/ProjectConfig.cmake` change `RTT_JLINK` to `RTT_LUART`
2. Rebuild the project
3. Catch logs: `picocom /dev/ttyACM0 -b 115200  --imap lfcrlf`

Please mind that logs on UART are more costly, so these might cause timing issues and slow down the phone too. To avoid that consider `release` build for tests with `uart` enabled.

## More details on preparing your local environment

### Code style with git hooks

The `bootstrap.sh` script installs git hooks for code style checking. `pre-commit.hook`automatically updates style during commit. If you haven't run `bootstrap.sh` and want to use git hooks, you have to copy (or link) `pre-commit.hook` to your git config directory `.git/config/hooks`: `ln -s `pwd`/config/pre-commit.hook .git/hooks/pre-commit`

By default, commit hook only checks if your changes have the appropriate style, if you would like to fix the style automatically during `git commit` you have to configure your git, by adding new variable `user.fixinstage` and setting it to `true` by running
`git config user.fixinstage true`

If you prefer "notification then fix" workflow (so you can examine the changes), use the default hook behaviour (for notifications) and then call `./config/pre-commit.hook --fix`, this checks and fixes files in "stage", files that have status "changed" are not tested.

```bash
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

Code style is done by clang

### Commit message template

To add a commit message template use this command:

```bash
git config commit.template .gitmessage
```

This way each time you add a new commit you will see the template that will help
you with the proper message format. More about that in [Development Workflow](development_workflow.md#commit-changes)

### Commit message hook
This hooks automatically converts your branch name to commit title

rules:
if branch starts with EGD-xxxx it is treated as jira ticket
all `-` and `_` are converted to spaces

#### Adding a hook:
```bash
cd .git/hooks
ln -s ../../config/prepare-commit-msg
```
#### Using a hook:

Create a branch

```bash
git checkout origin/master -b EGD-1234-Add_change_description
```

Do your changes, and prepare commit

```bash
git commit
```

## Build using Docker
You can build MuditaOS using a Docker container. To do that, follow these steps

1. Get Docker by running `./config/bootstrap.sh 8` or [install it yourself](https://www.docker.com/get-started) (if was not installed previously with `./config/bootstrap.sh 0-`).

After that you have to log out and log back in as groups are set during the login process. To make sure you are in docker group enter the `groups` command and 
you will see the list of groups your user is assigned to.

2. Configure for Linux Debug: 
```bash
./in_docker.sh config PurePhone|BellHybrid linux Debug
```

3. Build linux Debug:
```bash
./in_docker.sh make build-PurePhone|BellHybrid-linux-Debug
```

4. Build RT1051 Release
```bash
./in_docker.sh config PurePhone|BellHybrid rt1051 Release
./in_docker.sh make build-PurePhone|BellHybrid-rt1051-Release
```
5. Build and run tests (Linux only)

```bash
./in_docker.sh make build-PurePhone|BellHybrid-linux-Debug
./in_docker.sh make build-PurePhone|BellHybrid-linux-Debug check
```
To build selected test manually in the working directory of attached Docker image run:
```bash
cd ./build-PurePhone|BellHybrid-linux-Debug ; make <test_name>; ./<test_name>
```
i.e.:
```bash
cd ./build-PurePhone|BellHybrid-linux-Debug ; make unittest_gui && ./unittest_gui
```
Running tests checks memory leaks too - this might be helpful in creation of new widgets.

### Building your own Docker image

If for some reason (f.ex. testing new dependencies) you need to create your own docker image, you can do it by following:

1. Configure Docker file and download the necessary tool chain packages:

```bash
./config/build_runner_docker
```

2. Build a new Docker image

```bash
docker build docker -f docker/Dockerfile -t "wearemudita/mudita_os_builder:latest"
```

Please be aware that when building custom image you'll have to give it some tag (default from `build_runner_docker` is `latest`). You also need to provide some scripts with your tag (like `CONTAINER_TAG` in `in_docker.sh`), otherwise Docker will download and use original image from Mudita.

## Preparing packages

If you need a package, containing everything needed to run the application, please check [build_targests.md](build_targests.md) document.
