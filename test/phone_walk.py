#!/bin/python3
# script walking on desktop

from harness.common import Serial
import logging
import json

log = logging.getLogger(__name__)
logging.basicConfig(format='%(asctime)s %(message)s', level=logging.DEBUG)
conn = Serial()

for lol in range(1, 60 * 30):
    conn.key(ord('d'))
    conn.key(ord('s'))
    conn.key(ord('s'))
    conn.key(ord('d'))
