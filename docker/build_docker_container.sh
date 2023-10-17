#!/bin/bash
# Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set -euo pipefail
VERSION="1.19"

docker build --no-cache --tag "wearemudita/mudita_os_builder:${VERSION}" --target builder .
docker build --no-cache --tag "wearemudita/mudita_os_jenkins_slave:${VERSION}" --target jenkins-runner .