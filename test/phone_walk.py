#!/bin/python3
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# script walking on desktop

from interface.CDCSerial import CDCSerial

new_conn = CDCSerial()

for lol in range(1, 60 * 30):
    new_conn.send_key(ord('d'))
    new_conn.send_key(ord('s'))
    new_conn.send_key(ord('s'))
    new_conn.send_key(ord('d'))
