#!/usr/bin/env bash

# if there is no argument - then use Mati startup, else use mine :)
# mine uses other `-device` because my instalation of tools lost ours (but works fine with gdb)
# speed should be definetly configurable
# `mine` is continous (to be used together with `./run.sh`)
if [[ $# -lt 1 ]]; then
    JLinkGDBServerCLExe -if SWD -device MCIMXRT1051 \
        -speed 25000 -jlinkscriptfile "evkbimxrt1050_sdram_init.jlinkscript" -singlerun -strict -ir &
elif [[ $1 == "continous" ]]; then
    JLinkGDBServerCLExe -if SWD -device MIMXRT1064xxx5A \
        -endian little -port 2331 -swoport 2332 -telnetport 2333 -localhostonly 1 \
        -speed 16000 -jlinkscriptfile "evkbimxrt1050_sdram_init.jlinkscript" -strict -ir &
fi

