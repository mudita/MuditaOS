#!/usr/bin/env bash


JLinkGDBServerCLExe -if SWD -device MCIMXRT1051 -speed 25000 -jlinkscriptfile "evkbimxrt1050_sdram_init.jlinkscript" -singlerun -strict -ir &
