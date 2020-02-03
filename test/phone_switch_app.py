#!/bin/python3
# script walking on desktop
from harness.common import Serial
import logging
import json

log = logging.getLogger(__name__)
logging.basicConfig(format='%(asctime)s %(message)s', level=logging.DEBUG)
conn = Serial()


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
    conn.key(ord('d'))
    conn.key(ord('d'))
    conn.key(ord('\n'))
    data = cmd_json(conn.read(4))
    log.debug(data)
    conn.sleep(1)
    conn.key(12)
    conn.sleep(1)
    data = cmd_json(conn.read(4))
    log.debug(data)
