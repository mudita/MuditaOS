# Quickstart

You can quickstart the project by going through one of the following guides:

- [Introduction](#introduction)
- [Quickstart on Linux](#quickstart-on-linux)
- [Quickstart with unit tests](#quickstart-with-unit-tests)
- [Quickstart on the phone](#quickstart-on-the-phone)
- [More details on preparing your local environment](#more-details-on-preparing-your-local-environment)
- [CI build using Docker](#ci-build-using-docker)
- [Building your own Docker image](#building-your-own-docker-image)
- [Preparing packages](#preparing-packages)

## Introduction

Before running the scripts add yourself to a `docker` group:

```bash
NAME=$(whoami)
sudo usermod -aG docker ${NAME}
```

After that you have to log out and log back in as groups are set during the login process. To make sure you are in docker group enter the `groups` command and 
you will see the list of groups your user is assigned to.

Run the provisioning script `./config/bootstrap.sh` to install all dependencies. The script is written for Ubuntu and tested on 20.04.

To run the script execute the following command: `cd config && ./bootstrap.sh 0-`

After running provisioning you are ready to checkout and build project for both linux and RT1051. Please follow these steps:

1. Check out the project with submodules for the first time:

```bash
git submodule update --init --recursive
```
2. Update the repository with submodules

```bash
git pull --recurse-submodules
```
3. Build the project

```bash
./configure.sh [rt1051|linux] [release|debug|relwithdebinfo]
cd build-[rt1051|linux]-[release|debug|relwithdebinfo]
make
```

### Quickstart on Linux

Here's the bare minimum that will enable you to bootstrap the environment on Linux.

```bash
git submodule update --init --recursive                        # initialize submodules
cd ./config/ && ./bootstrap.sh 0- && cd ../                    # bootstrap requirements
./configure.sh rt1051|linux Debug|Release|RelWithDebInfo       # configure build
cd <build-dir>                                                 # build dir depends on configuration
make -j                                                        # build
./PurePhone                                                    # run PurePhone - emulator screen will pop up (on the Linux filesystem)
```

If you want to run the emulator with image and our VFS implementation
you need to run the image through the script:
```bash
./run_emulator_on_filesystem_image.sh
```

### Quickstart with unit tests

After going through the Super dirty quickstart on Linux, run the following commands to build and run all unit tests:

```bash
./configure.sh linux debug
cd <build-dir>
make check
```
### Quickstart on the phone

1. First install J-Link driver. We suggest using J-Link driver in version J-Link v634f ([Ubuntu download](https://www.segger.com/downloads/jlink/JLink_Linux_V634f_x86_64.deb))

2. Please run the following commands:

```bash
cp ./env.cmake.sample ./env.cmake && sed -i "s:<HOME>:$HOME:" env.cmake
git submodule update --init --recursive
cd ./config/ && ./bootstrap.sh 0- && cd ../
./configure.sh rt1051 RelWithDebInfo
cd build-arm-RelWithDebInfo
make -j

```

3. Please take the following steps in three parallel consoles/sessions:

 - Start J-Link server: `./StartJLinkServer.sh`
 - Load app with GDB via J-Link connected to Mudita Pure: `./run.sh <build folder>`
 - Catch logs from Mudita Pure from J-Link RTT and write them to `/tmp/log.txt`: `JLinkRTTClient | tee /tmp/log.txt`

#### Catching logs using UART

If you want to catch logs from Mudita Pure from UART please follow these steps:

1. In `config/ProjectConfig.cmake` change `RTT_JLINK` to `RTT_LUART`
2. Rebuild the project
3. Catch logs: `picocom /dev/ttyACM0 -b 115200  --imap lfcrlf`

Please mind that logs on UART are more costly, so these might cause timing issues and slow down the phone too. To avoid that consider `release` build for tests with `uart` enabled.

## More details on preparing your local environment

The quickest way to set up your environment is running the following command:
```bash
cd config && ./bootstrap.sh 0-
```
The script is written for Ubuntu and tested on 20.04.

* This script will require `sudo` (for `apt`)
* It will install `cmake` and `gcc` by default in `${HOME}` directory - in case of other needs, please change the script
* The script doesn't install `ccache`, but if `ccache` is on the system - its support is added to `env.cmake.sample`
* The script needs to be run only once

If the script is run after a fresh `git clone` you need to update your git config (step 0 and 1).

### `bootstrap.sh` steps, line by line

* `./config/bootstrap.sh 0`  - install style checking scripts to be automatically run on commit
* `./config/bootstrap.sh 1`  - `git blame` will ignore style changing commit
* `./config/bootstrap.sh 2`  - list packages required for setting up the environment *which are currently not installed*
* `./config/bootstrap.sh 3`  - set up arm toolchain, download and install it in home directory
* `./config/bootstrap.sh 4`  - set up `cmake`, download and install in home directory
* `./config/bootstrap.sh 5`  - list of commands required for switching default gcc/g++ to version 9
* `./config/bootstrap.sh 6`  - add paths for arm toolchain to your `PATH` environment variable - this is also used by `./env.cmake` *(this is required because `./env.cmake` uses environment variables set by this target)*
* `./config/bootstrap.sh 7`  - add paths for `cmake` to your `PATH` environment variable
* `./config/bootstrap.sh 8`  - install Docker

### Code style with git hooks

The `bootstrap.sh` script installs git hooks for code style checking. `pre-commit.hook`automatically updates style during commit. If you haven't run `bootstrap.sh` and want to use git hooks, you have to copy (or link) `pre-commit.hook` to your git config directory `.git/config/hooks`: `ln -s `pwd`/config/pre-commit.hook .git/hooks/pre-commit`

By default commit hook only checks if your changes have the appropriate style, if you would like to fix the style automatically during `git commit` you have to configure your git, by adding new variable `user.fixinstage` and setting it to `true` by running
`git config user.fixinstage true`

If you prefer "notification than fix" workflow (so you can examine the changes), use the default hook behaviour (for notifications) and then call `./config/pre-commit.hook --fix`, this checks and fixes files in "stage", files that have status "changed" are not tested.

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

### Commit message template

To add a commit message template use this command:

```bash
git config commit.template .gitmessage
```

This way each time you add a new commit you will see the template that will help
you with the proper message format. More about that in (Development Workflow)[./doc/development_workflow.md#commit-changes]

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

## CI build using Docker
You can build MuditaOS using a Docker container. To do that, follow these steps

1. Get Docker by running `./config/bootstrap.sh 8` or [install it yourself](https://www.docker.com/get-started) (if was not installed previously with `./config/bootstrap 0-`).

2. Add yourself to a `docker` group:

```bash
NAME=$(whoami)
sudo usermod -aG docker ${NAME}
```
After that you have to log out and log back in as groups are set during the login process. To make sure you are in docker group enter the `groups` command and 
you will see the list of groups your user is assigned to.

3. Configure for Linux Debug: 
```bash
./in_docker.sh config linux Debug
```

4. Build linux Debug:
```bash
./in_docker.sh make build-linux-Debug
```

5. Build RT1051 Release
```bash
./in_docker.sh config rt1051 Release
./in_docker.sh make build-rt1051-Release
```
6. Build and run tests (Linux only)

```bash
./in_docker.sh make build-linux-Debug
./in_docker.sh make build-linux-Debug check
```
To build selected test manually in the working directory of attached Docker image run:
```bash
cd ./build-linux-Debug ; make <test_name>; ./<test_name>
```
i.e.:
```bash
cd ./build-linux-Debug ; make unittest_gui && ./unittest_gui
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

If you need a package, containing everything needed to run the application (on target device or Linux), in the build directory run the following command: `make package`

After executing this command, the name of the package that was created will be displayed on the screen.

Package name is: `PurePhone-<version>-<target>.<extension>`
where:

- `<version>`   - is read from the latest "release-x.y.z" tag
- `<target>`    - RT1051 or Linux 
- `<extension>` - `zip` for RT1051 and `tar.gz` for Linux
