#!/usr/bin/env bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

declare -A OPTS=( ['continous']=' -singlerun ' ['verify']='  ' ['speed']=' -speed 25000 ')
jlinkScriptFileName="evkbimxrt1050_sdram_init.jlinkscript"

help()
{
cat << EOF
For basic run with default values:
./StartJLinkServer.sh
Params:
    continous   - run in continous mode
    verify      - verify data loaded
    speed       - change speed (please mind 30000 is max by docs)
    T6          - run for the T6 board 
EOF
}
while [[ $# -gt 0 ]]; do
    case $1 in
        "continous")
            echo "change to continous mode"
            OPTS[$1]=' ';
            ;;
        "verify")
            OPTS[$1]=' -vf ';
            ;;
        "speed")
            OPTS[$1]=" -speed $2 "
            shift
            ;;
        "T6")
            echo "run for the T6 board"
            jlinkScriptFileName="evkbimxrt1050_sdram_init_T6.jlinkscript"
            ;;
        *)
            help
            ;;
    esac;
    shift
done
CMD="/opt/JLink_Linux_V698e_x86_64/JLinkGDBServerCLExe -if SWD -device MCIMXRT1051                     \
    -jlinkscriptfile $jlinkScriptFileName -strict -ir    \
    -rtos GDBServer/RTOSPlugin_FreeRTOS                                  \
    ${OPTS[@]}"

echo "RUN: ${CMD}"

if [ "${OPTS['continous']}" == " " ]; then 

notify-send 'DEBUG WARNING!' "
When using continous mode, after full reset please remember to set PS_EN pin high. GDB command:

    monitor memU32 0x401BC000 = 128;

This will set PS_EN high so that after core reset power circuit would work while  loading software
ps. you can copy it from .gdbinit-rt1051
" \
--icon=dialog-information
fi

eval ${CMD}
