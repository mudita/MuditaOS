#!/bin/python3
# script walking on desktop
from common import *

ser = start()
registerCTRLP(ser)

for lol in range (1,60*30):
    writeKey(ser, ord('d'),0.3)
    writeKey(ser, ord('s'),0.3)
    writeKey(ser, ord('s'),0.3)
    writeKey(ser, ord('d'),0.3)

ser.close()
print("test end")
