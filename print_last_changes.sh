#!/bin/bash
# prints change log as a GitHub Variable

CHANGELOG_FILE="changelog.md"

function printChangeLogForGH() {
    CHANGELOG="$(cat ${CHANGELOG_FILE})"
    CHANGELOG="${CHANGELOG//'%'/'%25'}"
    CHANGELOG="${CHANGELOG//$'\n'/'%0A'}"
    CHANGELOG="${CHANGELOG//$'\r'/'%0D'}"
    echo "::set-output name=release_notes::${CHANGELOG}"
}

printChangeLogForGH
