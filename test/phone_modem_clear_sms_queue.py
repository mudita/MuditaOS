#!/bin/python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.uart import conn, log

conn.gsm("at+cmgl=\"all\"\r")
print(conn.read(10))
conn.gsm("at+cmgd=1,4\r")
conn.gsm("at+cmgd=0,4\r")
conn.gsm("at+cmgd=2,4\r")
conn.gsm("at+cmgl=\"all\"\r")
print(conn.read(10))
