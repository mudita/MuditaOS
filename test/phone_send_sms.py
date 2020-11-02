#!/bin/python3
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# get to sms from desktop and send sms ABC

import json
from harness.uart import conn, new_conn, log

def cmd_json(data):
    ret = {}
    try:
        ret = json.loads(data)
    except Exception as ex:
        print(ex)
    return ret

# get in sms
new_conn.send_key(ord('d'), 0.3)
new_conn.send_key(ord('d'), 0.3)
new_conn.send_key(ord('\n'), 0.3)
data = cmd_json(conn.read(4))
log.info(data)

# select nevest thread
new_conn.send_key(ord('\n'), 3)

# add sms ABC
# writeKey(ser, 1, 0.3)
new_conn.send_key(2, 3) # wait 3 sec -> so that needed assets for text processing would load bufor 100%
new_conn.send_key(3, 0.5)
new_conn.send_key(4, 0.5)

#send
new_conn.send_key(ord('\n'), 0.3)
