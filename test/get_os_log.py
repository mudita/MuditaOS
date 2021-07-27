#!/usr/bin/env python3
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import os.path
import atexit

from harness.harness import Harness
from harness.interface.error import TestError, Error
from harness.api.developermode import PhoneModeLock
from harness.api.filesystem import get_log_file


def set_passcode(harness: Harness, flag: bool):
    '''
    on exit -> restore PhoneModeLock
    '''
    PhoneModeLock(flag).run(harness)


def main():
    if len(sys.argv) == 1:
        print(
            f'Please pass log storage directory as the parameter: \'python {sys.argv[0]} <log dir>\' ')
        raise TestError(Error.OTHER_ERROR)

    log_dir = str(sys.argv[1])

    if (not os.path.exists(log_dir)):
        print(f'Log storage directory {log_dir} not found')
        raise TestError(Error.OTHER_ERROR)

    harness = Harness.from_detect()

    atexit.register(set_passcode, harness, True)
    set_passcode(harness, False)
    get_log_file(harness, log_dir)
    exit(0)


if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        print(err)
        exit(err.get_error_code())
