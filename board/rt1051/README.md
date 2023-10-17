
# board-rt1051

This directory contains low-level drivers and files necessary to configure and run RT1051.

## Files organisation

- Linker scripts for RT1051 `ldscripts/`
- Memory organisation
    - Pure `puretx/`
    - Bell `bellrx/`
- Crashdump
- Common Microcontroller Software Interface Standard (CMSIS)
- Newlib
    - GCC11
    - GCC12
- xip


## Crashdump
Crashdump helps to analyse crash dump files. If you consider change memory organisation for Pure or Bell
remember to change `crashdump/crashcatcher_impl.cpp`

Tool for analyse: https://github.com/mudita/misc-tools/tree/master/crashdump

