#!/usr/bin/env python
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md


from harness.harness import Harness
import sys


harness = Harness(sys.argv[1])

harness.connection.send_at("at+cmgl=\"all\"", 10000)
harness.connection.send_at("at+cmgd=1,4", 10000)
harness.connection.send_at("at+cmgd=0,4", 10000)
harness.connection.send_at("at+cmgd=2,4", 10000)
harness.connection.send_at("at+cmgl=\"all\"", 10000)

