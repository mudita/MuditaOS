# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from interface.defs import *
from test import *
import time


def sendKey(key_code):
    msg, result_msg = prepare_message(endpoint["developerMode"], method["put"], status["OK"],
                                      {"keyPressed": key_code}, None)
    test = Test(self.serial, msg, result_msg)
    test.execute()


class DeveloperModeTest:
    def __init__(self, serial):
        self.serial = serial.get_serial()

    def run(self):
        # unlock the phone
        sendKey(key_codes["enter"])
        time.sleep(1)
        sendKey(key_codes["#"])
        time.sleep(1)

        for i in range(4):
            sendKey(3)
            time.sleep(1.5)

        return True
