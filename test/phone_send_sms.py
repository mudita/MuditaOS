#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# get to sms from desktop and send sms ABC


from harness.uart import connection

# get in sms
connection.send_key(ord('d'), 0.3)
connection.send_key(ord('d'), 0.3)
connection.send_key(ord('\n'), 0.3)

# select newest thread
connection.send_key(ord('\n'), 3)

# add sms ABC
# writeKey(ser, 1, 0.3)
connection.send_key(2, 3) # wait 3 sec -> so that needed assets for text processing would load bufor 100%
connection.send_key(3, 0.5)
connection.send_key(4, 0.5)

#send
connection.send_key(ord('\n'), 0.3)
