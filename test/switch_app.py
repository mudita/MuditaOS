#!/bin/python3
# script walking on desktop
from common import *
import json

# init
ser = start()
registerCTRLP(ser)

def cmd_json(data):
    ret = {}
    try:
        ret = json.loads(data)
    except Exception as ex:
        print(ex)
    return ret

# get in sms and back 100 times
for el in range(1,100):
    print("TEST ex: {}".format(el))
    # back
    # move left , enter, read for entry
    writeKey(ser, ord('d'), 0.3)
    writeKey(ser, ord('d'), 0.3)
    writeKey(ser, ord('\n'), 0.3)
    data = cmd_json(read_cmd(ser, 4))
    print(data)
    time.sleep(1)
    writeKey(ser, 12, 0.3)
    time.sleep(1)
    data = cmd_json(read_cmd(ser, 4))
    print(data)

ser.close()
