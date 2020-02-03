![alt text](./Images/mudita_logo.png "MUDITA")

# How to setup environment

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
See here: [**doc/config.clion.md**](./config.clion.md)

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