#!/usr/bin/awk -f
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

BEGIN {
    version_count=0
}

/^## / {
    version_count++
}

{
    if(version_count < 2) {
        print $0
    }
}
