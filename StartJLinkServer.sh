#!/usr/bin/env bash

declare -A OPTS=( ['continous']=' -singlerun ' ['verify']='  ' ['speed']=' -speed 25000 ')

help()
{
cat << EOF
For basic run with default values:
./StartJLinkServer.sh

Params:
    continous   - run in continous mode
    verify      - verify data loaded
    speed       - change speed (please mind 30000 is max by docs)
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
        *)
            help
            ;;
    esac;
    shift
done

CMD="JLinkGDBServerCLExe -if SWD -device MCIMXRT1051                     \
    -jlinkscriptfile evkbimxrt1050_sdram_init.jlinkscript -strict -ir    \
    -rtos GDBServer/RTOSPlugin_FreeRTOS                                  \
    ${OPTS[@]}"

echo "RUN: ${CMD}"

eval ${CMD}
