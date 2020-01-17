#!/bin/python3
# unlock Desktop
from common import *

ser = start()
registerCTRLP(ser)

# write pin
writeKey(ser, 10, 0.5)
writeKey(ser, 35, 0.5)
writeKey(ser, 3, 0.5)
writeKey(ser, 3, 0.5)
writeKey(ser, 3, 0.5)
writeKey(ser, 3, 0.5)
writeKey(ser, 10, 0.5)
# enter menu
writeKey(ser, 10, 1.5)

ser.close()
