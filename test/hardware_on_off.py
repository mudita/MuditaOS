# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import subprocess
import os
import sys
import time

from harness import log
from harness.harness import Harness
from harness.interface.error import TestError, Error

def boot_phone(err_file):
    return subprocess.Popen(['gnome-terminal', '--', 'sh', './bootSimulator.sh', err_file])

def check_for_errors(err_file):
    search_words = ['ABORTING', 'terminate']

    with open(err_file) as f:
        if any(word in  f.read() for word in search_words):
            return True
    return False

def run_test(port_name):
    harness = Harness(port_name)
    harness.unlock_phone()
    harness.turn_phone_off()




def main():
    if len(sys.argv) == 1 or "/dev" not in sys.argv[1]:
        log.warning("Port name not passed, trying port name filename from simulator...")
        try:
            file = open("/tmp/purephone_pts_name", "r")
        except FileNotFoundError:
            raise TestError(Error.PORT_FILE_NOT_FOUND)

        port_name = file.readline()
        if port_name.isascii():
            log.debug("found {} entry!".format(port_name))
        else:
            print(f'Please pass port name as the parameter: python {sys.argv[0]} /dev/ttyACM0 number duration')
            raise TestError(Error.PORT_NOT_FOUND)
    else:
        port_name = sys.argv[1]

    err_file='logs.txt'

    boot_phone(err_file)
    time.sleep(3)
    run_test(port_name)
    time.sleep(10)
    result = check_for_errors(err_file)

    os.remove(err_file)
    if result:
        raise TestError(Error.TEST_FAILED)


if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        log.error(err)
        exit(err.get_error_code())
