# How to set up integrated development environment

## Table of contents
1. [Eclipse](#Eclipse)
2. [CLion](#CLion)
3. [Environment](#Environment)
4. [Separate build folders](#Separate)


### Eclipse <a name="Eclipse"></a>

**To run project in eclipse:**

* Run cmake to create build directory. Create inside build "sys" folder and copy all assets to it (e.g path for font: sys/assets/fonts/gt_pressura_regular_16.mpf ). Faster way is to run following command in main directory: **./rebuild script**

* Open projects properties and in tab 'C/C++ Build'->'Builder Settings' set build directory to: **${workspace_loc:/PurePhone}/build**
* Create deubg configuration ( right click on project and select debug as: local c/c++ application )
* Open new debug configuration and in Arguments tab set 'Working directory' to **${workspace_loc:PurePhone/build}**
* In Debugger tab set GDB command file to **${project_loc}/.gdbinit**

### CLion <a name="CLion"></a>
## setup Toolchains
- Open settings (CTRL+ALT+S)
- Goto to **Build, Execution, Deployment** → **Toolchains**
#### Add new toolchains (ALT+Ins) of type "system"
 - **Name**: Linux
 - CMake should be autodetected. if not `which cmake`
 - Select appropriate `gdb`
 - for x86 usualy `/usr/bin/gdb`
 
 and
 - **Name**: rt1051
 - CMake should be autodetected. if not `which cmake`
 - Select appropriate `gdb`
 - for rt1051 usualy `~/gcc-arm-eabi…`

## build configurations
- Open settings (CTRL+ALT+S)
- Goto to **Build, Execution, Deployment** → **CMake**
- Add new CMake Profile (ALT+Ins)
  - **Name**: Linux
  - **Build type**: Debug
  - **CMake options**:
    ```
    -DCMAKE_EXPORT_COMPILE_COMMANDS=True
    -DCMAKE_BUILD_TYPE=Debug
    -DCMAKE_TOOLCHAIN_FILE=../Target_Linux.cmake 
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1
    ```
  - Select appropriate Toolchain created above
  - Generation path: `build-linux`
  - build options: `-j8`

##### Note: you can set Generation path to `build` if you don't plan to configure CLion for rt1051.

and

- Add new CMake Profile (ALT+Ins)
  - **Name**: rt1051
  - **Build type**: Debug
  - **CMake options**:
    ```
    -DCMAKE_EXPORT_COMPILE_COMMANDS=True
    -DCMAKE_BUILD_TYPE=Debug
    -DCMAKE_TOOLCHAIN_FILE=../Target_RT1051.cmake
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 'DBT_STACK_SEL="Bluetopia"' ../
    ```
  - Select appropriate Toolchain created above
  - Generation path: `build-rt1051`
  - build options: `-j8`

# Running/debugging
## Linux configuration
**Important**
> Before 1st run, **Build All in 'Linux'** to generate `service_renderer` utility.

Go to **Edit Configurations…** (double shift) and select CMakeApplication → PurePhone. This is your configuration for Linux executable.
In **Working directory** append `/build`.
Now you can run/debug this configuration.

## rt1051 configuration
### prerequisites
Use attached file `.idea/JLink_server.xml` to setup IDE. It is already in the project folder, so CLion should pick it up by itself.
You'll also need `.gdbinit-1051-clion` file modified for CLion (source: https://stackoverflow.com/questions/39810593/gdb-monitor-commands-in-clion).
This files *needs* to be in `$HOME/.gdbinit`. 
For me `ln -s %project_root%/.gdbinit-1051-clion ~/.gdbinit` works.

## workflow
in the debuging configuration dropdown select a pair of **<exec>** and **<toolchn>** (**[ \<exec\> | \<toolchn\> \]**):
- **\[PurePhone | Linux\]** for run/debug on desktop (Linux)
- **\[JLink server | rt1051\]** for debug on embedded (rt1051)

**Note:** rt1051 doesn't have *run* configuration. Only debug is available.

![Profit](https://user-images.githubusercontent.com/56958031/71167426-f28ac780-2254-11ea-8efb-8375fb004004.png)

### running target
It is possible to see logs from a rt1051 booted from eMMC and not from RAM. 
To do so select **JLink server (no upload)** configuration.
Open `JLinkRTTClient` in a separate terminal.

Note: remember to **stop** this debug session when you want to switch to regular **JLink server** (with upload), otherwise they will collide.

True attaching to a running target (without a reset) is not known to be working.

# separate builds
To build separately for each arch we need to:
- Open settings (CTRL+ALT+S)
- Goto to **Build, Execution, Deployment** → **CMake**
- select `Linux`
- add a variable to **Environment**:
    - `PROJECT_BUILD_DIRECTORY=build-linux`
- change **Generation Path** to:
    - `build-linux`
    
and 

- select `rt1051`
- add a variable to **Environment**:
    - `PROJECT_BUILD_DIRECTORY=build-rt1051`
- change **Generation Path** to:
    - `build-rt1051`

---
And change Run/debug **Working directory** for Linux configuration. (**Edit configurations…** → **PurePhone**)
- change **Working directory** from `…/PurePhone/build` to `…PurePhone/build-linux`


### Environment <a name="Environment"></a>

If you want to prevent git from suggesting you to commit changes to the configuration files where we place for example local paths please cast following spell. 

**git update-index --skip-worktree env.cmake**

This will tell git to ignore local changes to the file. In above example we are preventing commiting changes to file called env.cmake where you probably want to place local path to arm's toolchain.

### Separate build folders<a name="Separate">
By default both targets (Linux & rt1051) are build into the same destination (**build** folder).
With this configuration you have to `./rebuild.sh` every time you want to change target. Also you cannot debug simultaneously Linux & rt1051.

There is an option for both targets to have _separate_ build folders.
To do so, set environmental variable `SEPARATE_BUILDS` with any value (e.g. `=1`) to `./rebuild.sh`.
#### Example:
`env SEPARATE_BUILDS=1 ./rebuild.sh rt1051` will cause rt1051 to be build into a separate `build-rt1051` folder.

By default `./rebuild.sh rt1051` builds into `build`)

---
You can build only linux build to its separate folder, and leave rt1051 in place, thus maintain compatibility with `./run.sh`. See below:

#### Example 2:
```
> env SEPARATE_BUILDS=1 ./rebuild.sh linux
> cd build-linux; ./PurePhone.elf
> cd ..
> ./rebuild.sh rt1051   # will be built to the default `build` folder
> StartJLinkServer.sh &
> ./run.sh
```

Guide for **[CLion](#CLion)** has a section on _separate builds_.

How it works:
By default `./rebuild.sh` copies assets and generates CMakes to generic `build`.
`SEPARATE_BUILDS=1` overrides it.

Then comes CMake. CMake uses persistent (CACHED) variable `PROJECT_BUILD_DIRECTORY` to build every module. 
You can set it either in `cmake-gui` or by passing environmental variable to `cmake`.
 

This works like a toggle. You don't have to specify it every time. The new build path gets embedded into every `CMakeCache.txt`, so any further `make` doesn't require any extra arguments.