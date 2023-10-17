#!/usr/bin/env bash
set -ex

REPO_PATH="/home/bartek/MuditaOS"
DST_VERSION=1.17

./config/build_runner_docker
docker build ${REPO_PATH}/docker -f ${REPO_PATH}/docker/Dockerfile -t "wearemudita/mudita_os_builder:${DST_VERSION}"
docker tag "wearemudita/mudita_os_builder:${DST_VERSION}" wearemudita/mudita_os_builder:latest
docker push "wearemudita/mudita_os_builder:${DST_VERSION}"
docker push wearemudita/mudita_os_builder:latest

docker build ${REPO_PATH}/docker/jenkins-docker -f ${REPO_PATH}/docker/jenkins-docker/Dockerfile -t "wearemudita/mudita_os_jenkins_slave:${DST_VERSION}"
docker tag "wearemudita/mudita_os_jenkins_slave:${DST_VERSION}" wearemudita/mudita_os_jenkins_slave:latest
docker push "wearemudita/mudita_os_jenkins_slave:${DST_VERSION}"
docker push wearemudita/mudita_os_jenkins_slave:latest