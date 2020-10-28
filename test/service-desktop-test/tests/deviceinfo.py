# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from interface.defs import *
from test import *


class DeviceInfoTest:
    def __init__(self, serial):
        self.serial = serial.get_serial()

    def run(self):

        msg, result_msg = prepare_message(endpoint["deviceInfo"], method["get"], status["OK"], {})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()

        if int(result["fsFreePercent"]) == 0 or int(result["fsFreePercent"]) > 100:
            return False
        if int(result["batteryState"]) != 1:
            return False

        return True
