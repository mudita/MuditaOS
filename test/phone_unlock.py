#!/bin/python3
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# unlock Desktop
from interface.CDCSerial import CDCSerial
from interface.defs import key_codes

new_conn = CDCSerial()
# write pin
new_conn.send_key(key_codes["enter"])
new_conn.send_key(key_codes["#"])
new_conn.send_key(3)
new_conn.send_key(3)
new_conn.send_key(3)
new_conn.send_key(3)

