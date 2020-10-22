#!/bin/python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.uart import conn, log

conn.gsm("at+cmgl=\"all\"")
print(conn.read(10))
conn.gsm("at+cmgd=1,4")
conn.gsm("at+cmgd=0,4")
conn.gsm("at+cmgd=2,4")
conn.gsm("at+cmgl=\"all\"")
print(conn.read(10))
