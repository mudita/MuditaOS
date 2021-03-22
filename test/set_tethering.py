# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys

from harness import log
from harness.harness import Harness
from harness.interface.error import TestError, Error


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

    harness = Harness(port_name)
    harness. unlock_usb()
    state = int(sys.argv[2])

    harness.unlock_phone()
    harness.set_tethering_state(True if state == 1 else False)
    # No response is available immediately.
    print(f'Please wait for the request to be sent...')
    time.sleep(30)


if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        log.error(err)
        exit(err.get_error_code())
