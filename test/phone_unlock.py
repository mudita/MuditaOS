#!/usr/bin/env python
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# unlock Desktop
from harness.harness import Harness

harness = Harness.from_detect()


@harness.with_phone_unlocked
def done(connection):
    pass
