#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# script walking on desktop
import time
from harness.uart import connection, log


# get in sms and back 100 times
for el in range(1, 100):
    log.info("TEST ex: {}".format(el))
    # back
    # move left , enter, read for entry
    connection.send_key(ord('d'))
    connection.send_key(ord('d'))
    connection.send_key(ord('\n'))
    time.sleep(1)
    connection.send_key(12)
    time.sleep(1)
