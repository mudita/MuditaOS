#!/bin/python3
# Phone wait for system start + 8 sec for GSM to end it's work and unlock
from common import *

ser = start()
registerCTRLP(ser)
data = read_cmd(ser,30)
print("SYS Started! {}".format(data))
time.sleep(8)
ser.close()
print("test end")
