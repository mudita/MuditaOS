#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.uart import connection

print(connection.AT("at+cmgl=\"all\""))

print(connection.AT("at+cmgd=1,4"))
print(connection.AT("at+cmgd=0,4"))
print(connection.AT("at+cmgd=2,4"))
print(connection.AT("at+cmgl=\"all\""))

