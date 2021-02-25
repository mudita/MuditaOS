#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.uart import connection, log

results = connection.send_at("AT+CPIN?", 10000)
for result in results:
    print(result)
