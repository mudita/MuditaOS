#!/usr/bin/env bash

declare JOB

## run command in the background
background() {
  eval $1 & JOB=$!
}

## returns jobs status if job failed
reap() {
  local status=0
    wait ${JOB} ; status=$?
    if [[ $status -ne 0 ]]; then
      echo -e "[${JOB}] Exited with status: ${status}"
    fi
  return ${status}
}

cd ..
background './run_emulator_on_filesystem_image.sh 2>&1 > emulator.log'
echo $! > emulator.pid

reap || echo "Ooops! Some jobs failed"
