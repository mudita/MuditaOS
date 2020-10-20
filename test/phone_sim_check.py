#!/bin/python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.uart import conn, log

conn.gsm("AT+CPIN?\r");
print(conn.read(10))
