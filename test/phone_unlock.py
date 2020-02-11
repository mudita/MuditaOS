#!/bin/python3
# unlock Desktop
from harness.common import Serial
import logging
logging.basicConfig(format='%(asctime)s %(message)s', level=logging.DEBUG)
log = logging.getLogger(__name__)

conn = Serial()

# write pin
conn.key(ord('\n'))
conn.key(ord('#'))
conn.key(3)
conn.key(3)
conn.key(3)
conn.key(3)
conn.key(ord('\n'))
# enter menu
conn.key(ord('\n'))
