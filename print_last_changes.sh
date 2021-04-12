#!/bin/bash
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# prints change log as a GitHub Variable

CHANGELOG_FILE="changelog.md"

function printChangeLogForGH() {
    CHANGELOG="$(awk -f tools/last_changelog.awk ${CHANGELOG_FILE})"
    CHANGELOG="${CHANGELOG//'%'/'%25'}"
    CHANGELOG="${CHANGELOG//$'\n'/'%0A'}"
    CHANGELOG="${CHANGELOG//$'\r'/'%0D'}"
    echo "::set-output name=release_notes::${CHANGELOG}"
}

printChangeLogForGH
