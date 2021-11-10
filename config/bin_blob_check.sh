#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set -euo pipefail

for f in $(git diff --name-only remotes/origin/master...HEAD); do
    case $f in
       *.mpf|Luts.bin|*gt_pressura*)
           echo "file $f is not accepted binary blob for repository"
           exit 1;
           ;;
    esac
done
