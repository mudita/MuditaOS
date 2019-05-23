![alt text](./Images/mudita_logo.png "MUDITA")

# How to setup environment

## Table of contents
1. [Eclipse](#Eclipse)
2. [Environment](#Environment)


### Eclipse <a name="Eclipse"></a>

**To run project in eclipse:**

* Run cmake to create build directory. Create inside build "sys" folder and copy all assets to it (e.g path for font: sys/assets/fonts/gt_pressura_regular_16.mpf ). Faster way is to run following command in main directory: **./rebuild script**

* Open projects properties and in tab 'C/C++ Build'->'Builder Settings' set build directory to: **${workspace_loc:/PurePhone}/build**
* Create deubg configuration ( right click on project and select debug as: local c/c++ application )
* Open new debug configuration and in Arguments tab set 'Working directory' to **${workspace_loc:PurePhone/build}**
* In Debugger tab set GDB command file to **${project_loc}/.gdbinit**

### Environment <a name="Environment"></a>

If you want to prevent git from suggesting you to commit changes to the configuration files where we place for example local paths please cast following spell. 

**git update-index --skip-worktree env.cmake**

This will tell git to ignore local changes to the file. In above example we are preventing commiting changes to file called env.cmake where you probably want to place local path to arm's toolchain.