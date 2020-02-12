#!/bin/python
from harness.uart import conn, log

# conn.gsm("AT+QSIMDET=?\r")
# conn.read(1)
# conn.sleep(3)
# conn.gsm("AT+QSIMSTAT=?\r")
# conn.read(1)
# conn.sleep(3)

# disable and check SIM
conn.gpio({"num": 1, "state": 1})
conn.sleep(1)
# right now qsim is on high trigger (...)
conn.gpio({"num": 1, "state": 0})
conn.sleep(1)
