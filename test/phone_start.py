#!/bin/python3
# Phone wait for system start + 8 sec for GSM to end it's work and unlock
from common import *

ser = start()
data = read_cmd(ser)
print("SYS Started! {}".format(data))

time.sleep(8)

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
print("test end")
