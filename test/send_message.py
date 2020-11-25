#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import time

from harness import log
from harness.harness import Harness
from harness.interface.defs import key_codes, endpoint, method
from harness.interface.error import TestError, Error


def send_message(harness, phone_number: str, message: str):
    @harness.with_phone_unlocked
    def send(connection):
        # enter menu
        connection.send_key(key_codes["enter"])
        harness.open_application("messages")
        if harness.connection.get_window() != "ApplicationMessages":
            time.sleep(2)
            if harness.connection.get_window() != "ApplicationMessages":
                print("Application didn't switch, exiting...")
                exit(1)

        # create new message
        connection.send_key(key_codes["left"])
        # enter phone number
        harness.send_number(phone_number)
        # move down to message body
        connection.send_key(key_codes["down"])
        # write a message
        harness.send_text(message)
        # send
        connection.send_key(key_codes["enter"])


def get_message_by_text(harness, message: str):
    body = {"messageBody": message}
    return harness.endpoint_request("messages", "get", body)


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
            print(f'Please pass port name as the parameter: python {sys.argv[0]} /dev/ttyACM0 number text ')
            raise TestError(Error.PORT_NOT_FOUND)
    else:
        port_name = sys.argv[1]

    harness = Harness(port_name)
    message = str(sys.argv[3])
    messages = get_message_by_text(harness, message.upper())

    send_message(harness, str(sys.argv[2]), message)
    time.sleep(2)
    new_messages = get_message_by_text(harness, message.upper())

    diff = [i for i in messages + new_messages if i not in messages or i not in new_messages]
    if len(diff) != 1 or diff[0]["type"] != 0x08:  # 0x08 - SMSType::OUTBOX
        log.error("sending error!")
        raise TestError(Error.TEST_FAILED)
    else:
        log.info("sending success!")


if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        log.error(err)
        exit(err.get_error_code())

