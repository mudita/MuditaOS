#!/usr/bin/env python
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import time

from harness import log
from harness.harness import Harness
from harness.interface.defs import key_codes, endpoint, method
from harness.interface.error import TestError, Error


def call(harness, phone_number: str, duration: int):
    @harness.with_phone_unlocked
    def do_it(connection):
        # enter number
        harness.send_number(phone_number)
        # call
        connection.send_key_code(key_codes["fnLeft"])

        time.sleep(duration)
        connection.send_key_code(key_codes["fnRight"])


def get_calllog_count(harness):
    body = {"count": True}
    return harness.endpoint_request("calllog", "get", body)["body"]["count"]


def main():
    if len(sys.argv) == 1 or "/dev" not in sys.argv[1]:
        log.warning("Port name not passed, trying port name filename from simulator...")
        try:
            file = open("/tmp/purephone_pts_name", "r")
        except FileNotFoundError as err:
            raise TestError(Error.PORT_FILE_NOT_FOUND)

        port_name = file.readline()
        if port_name.isascii():
            log.debug("found {} entry!".format(port_name))
        else:
            print(f'Please pass port name as the parameter: python {sys.argv[0]} /dev/ttyACM0 number duration')
            raise TestError(Error.PORT_NOT_FOUND)
    else:
        port_name = sys.argv[1]

    harness = Harness(sys.argv[1])
    harness.unlock_phone()
    number = str(sys.argv[2])
    duration = int(sys.argv[3])
    count_before = get_calllog_count(harness)
    call(harness, number, duration)
    count_after = get_calllog_count(harness)

    if count_after == count_before + 1:
        print("Success!")
    else:
        print("Error!")
        raise TestError(Error.TEST_FAILED)


if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        log.error(err)
        exit(err.get_error_code())
