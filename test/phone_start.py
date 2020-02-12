#!/bin/python3
# Phone wait for system start + 8 sec for GSM to end it's work and unlock
from harness.common import Serial
import logging
logging.basicConfig(format='%(asctime)s %(message)s', level=logging.DEBUG)
log = logging.getLogger(__name__)

conn = Serial()
data = conn.read(30)
log.debug("SYS Started! {}".format(data))
conn.sleep(8)
