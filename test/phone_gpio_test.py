#!/usr/bin/env python
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.uart import conn, log

# conn.gsm("AT+QSIMDET=?")
# conn.read(1)
# conn.sleep(3)
# conn.gsm("AT+QSIMSTAT=?")
# conn.read(1)
# conn.sleep(3)

# disable and check SIM
conn.gpio({"num": 1, "state": 1})
conn.sleep(1)
# right now qsim is on high trigger (...)
conn.gpio({"num": 1, "state": 0})
conn.sleep(1)
