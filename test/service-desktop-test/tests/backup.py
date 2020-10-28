# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from interface.defs import *
from test import *


class BackupTest:
    def __init__(self, serial):
        self.serial = serial.get_serial()

    def run(self):

        msg, result_msg = prepare_message(endpoint["backup"], method["get"], status["OK"],
                                          {"backupReady": True}, {"backupReady": False})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            if result["backupReady"] == True:
                print("backup already ready!")
            else:
                return False

        msg, result_msg = prepare_message(endpoint["backup"], method["get"], status["OK"],
                                          {}, {"backupRequest": False})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        msg, result_msg = prepare_message(endpoint["backup"], method["get"], status["OK"],
                                          {"backupRequest": True}, {"backupRequest": True})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        msg, result_msg = prepare_message(endpoint["backup"], method["get"], status["OK"],
                                          {"backupReady": True}, {"backupReady": True})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        msg, result_msg = prepare_message(endpoint["backup"], method["post"], status["OK"],
                                          {"backupUpload": True}, {"backupUpload": False})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        return True
