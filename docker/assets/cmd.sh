#!/usr/bin/env bash
set -e

if [ -f settings/.runner -a -f settings/.credentials ]; then
  cp settings/.runner .runner
  cp settings/.credentials .credentials
else
  ./config.sh  \
    --unattended \
    --url $REPOSITORY_URL \
    --token $WORKER_TOKEN \
    --name $WORKER_NAME \
    --replace
  cp .runner settings/.runner
  cp .credentials settings/.credentials
fi

nohup ./run.sh &
trap 'exit 0' INT TERM
while true; do sleep 1; done
