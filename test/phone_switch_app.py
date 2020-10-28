#!/bin/python3
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# script walking on desktop
from harness.uart import conn, new_conn, log
import json

def cmd_json(data):
    ret = {}
    try:
        ret = json.loads(data)
    except Exception as ex:
        print(ex)
    return ret


# get in sms and back 100 times
for el in range(1, 100):
    log.info("TEST ex: {}".format(el))
    # back
    # move left , enter, read for entry
    new_conn.send_key(ord('d'))
    new_conn.send_key(ord('d'))
    new_conn.send_key(ord('\n'))
    data = cmd_json(conn.read(4))
    log.debug(data)
    conn.sleep(1)
    new_conn.send_key(12)
    conn.sleep(1)
    data = cmd_json(conn.read(4))
    log.debug(data)
