#!/bin/python3
# get to sms from desktop and send sms ABC

import json
from harness.uart import conn, log

def cmd_json(data):
    ret = {}
    try:
        ret = json.loads(data)
    except Exception as ex:
        print(ex)
    return ret

# get in sms
conn.key(ord('d'), 0.3)
conn.key(ord('d'), 0.3)
conn.key(ord('\n'), 0.3)
data = cmd_json(conn.read(4))
log.info(data)

# select nevest thread
conn.key(ord('\n'), 3)

# add sms ABC
# writeKey(ser, 1, 0.3)
conn.key(2, 3) # wait 3 sec -> so that needed assets for text processing would load bufor 100%
conn.key(3, 0.5)
conn.key(4, 0.5)

#send
conn.key(ord('\n'), 0.3)
