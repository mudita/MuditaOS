#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# unlock Desktop
from harness.uart import connection
from interface.defs import key_codes

# write pin
connection.send_key(key_codes["enter"])
connection.send_key(key_codes["#"])
connection.send_key(3)
connection.send_key(3)
connection.send_key(3)
connection.send_key(3)

