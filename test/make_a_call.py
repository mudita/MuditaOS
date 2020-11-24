#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import time

from harness.harness import Harness
from harness.interface.defs import key_codes, endpoint, method


def call(harness, phone_number: str, duration: int):
    @harness.with_phone_unlocked
    def do_it(connection):
        # enter number
        harness.send_number(phone_number)
        # call
        connection.send_key(key_codes["fnLeft"])

        time.sleep(duration)
        connection.send_key(key_codes["fnRight"])


def get_calllog_count(harness):
    body = {"count": True}
    return harness.endpoint_request("calllog", "get", body)["count"]


def main():
    if len(sys.argv) == 1 or sys.argv[1] is None:
        print(f'Please pass port name as the parameter: python {sys.argv[0]} /dev/ttyACM0 number duration')
        exit(1)

    harness = Harness(sys.argv[1])
    number = str(sys.argv[2])
    duration = int(sys.argv[3])
    count_before = get_calllog_count(harness)
    call(harness, number, duration)
    count_after = get_calllog_count(harness)

    if count_after == count_before +1:
        print("Success!")
    else:
        print("Error!")
        exit(1)


if __name__ == "__main__":
    main()
