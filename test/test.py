#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
from harness.harness import Harness
from harness.interface.defs import key_codes


def main():
    if len(sys.argv) == 1 or sys.argv[1] is None:
        print('Please pass port name as the parameter: python test.py /dev/ttyACM0 ')
        exit(1)

    test = Harness(sys.argv[1])

    print(test.connection.get_window())

    @test.with_phone_unlocked
    def decorator_test(connection):
        # enter menu
        connection.send_key(key_codes["enter"])

    print(test.connection.get_window())

    test.open_application("settings")
    print(test.connection.get_window())


if __name__ == "__main__":
    main()
