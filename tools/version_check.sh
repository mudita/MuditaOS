#!/bin/bash
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set -euo pipefail
#set -o posix;
LC_CTYPE="C"

# WARNING: depends on word splitting
semver2num () { printf "%03.0f%03.0f%03.0f" $(echo "$1" | sed -e "s/[^[:digit:]]*\([[:digit:]]\+\)\.\([[:digit:]]\+\)\.\([[:digit:]]\+\).*/\\1 \\2 \\3/"); }

# first argument is the "binname --version" output
# second argument is the minimum acceptable version
less_than_version () { expr $(semver2num "$1") "<" $(semver2num "$2"); }
greater_than_version () { expr $(semver2num "$1") ">" $(semver2num "$2"); }
