#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.uart import connection

connection.gsm("at+qsimdet=1,0", 10)
connection.gsm("AT+QSIMSTAT=1", 10)
connection.gsm("at&w", 10)
