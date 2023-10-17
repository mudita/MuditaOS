#!/usr/bin/env bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

declare JOB

## run command in the background
background() {
  eval $1
  JOB=$!
}

## returns jobs status if job failed
reap() {
  local status=0
    wait ${JOB}
    status=$?
    if [[ $status -ne 0 ]]; then
      echo -e "[${JOB}] Exited with status: ${status}"
    fi
  exit ${status}
}

pushd ..
background './run_simulator_on_filesystem_image.sh 2>&1 > simulator.log & echo $! > simulator.pid'
popd
sleep 1
eval 'python3 phone_off.py -rP --port=simulator --timeout=20 -m "not rt1051"'

reap
