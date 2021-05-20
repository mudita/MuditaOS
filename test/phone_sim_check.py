#!/usr/bin/env python
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md


from harness.harness import Harness
import sys

harness = Harness(sys.argv[1])

results = harness.connection.send_at("AT+CPIN?", 10000)
for result in results:
    print(result)
