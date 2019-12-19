# Clion configuration
## setup Toolchains
- Open settings (CTRL+ALT+S)
- Goto to "Build, Execution, Deployment", "**Toolchains**"
- Add new toolchains (ALT+Ins) of type "system"
 - Linux
 - rt1051
- Cmake should be autodetected. if not `which cmake`
- Select appropriate `gdb`
 - for x86 usualy `/usr/bin/gdb`
 - for rt1051 usualy `~/gcc-arm-eabi…`

## build configurations
- Open settings (CTRL+ALT+S)
- Goto to "Build, Execution, Deployment", "**CMake**"
- Add new CMake Profile (ALT+Ins)
 - Linux
 - rt1051
- Build type: Debug
- CMake options:
 - Linux:
 ```
 -DCMAKE_EXPORT_COMPILE_COMMANDS=True
 -DCMAKE_BUILD_TYPE=Debug
 -DCMAKE_TOOLCHAIN_FILE=../Target_Linux.cmake 
 -DCMAKE_EXPORT_COMPILE_COMMANDS=1
 ```
 - rt1051:
 ```
 -DCMAKE_EXPORT_COMPILE_COMMANDS=True
 -DCMAKE_BUILD_TYPE=Debug
 -DCMAKE_TOOLCHAIN_FILE=../Target_RT1051.cmake
 -DCMAKE_EXPORT_COMPILE_COMMANDS=1 'DBT_STACK_SEL="Bluetopia"' ../
 ```
- Select appropriate Toolchain created above
- Generation path: `build`
- build options: `-j4`

Side note: I (Mateusz) try to build Linux and ARM separately, so I don't need to `clean` at every arch change. I use `build-linux` and `build-rt1051` folders as Generation path. It works except for some `service_renderer`, which still builds in hardcoded `build` folder.

## debugging 
### conf
Use attached file `.idea/JLink_server.xml` to setup IDE. CLion should do it by itself.
You'll also need `.gdbinit-1051-clion` file modified for CLion (source: https://stackoverflow.com/questions/39810593/gdb-monitor-commands-in-clion).
This files *needs* to be in `$HOME/.gdbinit`. 
For me `ln -s %project_root%/.gdbinit-1051-clion ~/.gdbinit` works.

### flow
in the debuging configuration dropdown select a pair:
- `[PurePhone | Linux]` for desktop (Linux)
- `[JLink server | rt1051]` for embedded (rt1051)

For `[JLink server | rt1051]` you will only have Debug option, no Run.



<tu wstaw zdjęcie>