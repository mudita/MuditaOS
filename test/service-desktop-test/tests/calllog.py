# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
from harness.interface.defs import endpoint, method, status

from test_api import *


class CalllogTest:
    def __init__(self, serial):
        self.serial = serial.get_serial()

    def run(self):

        msg, result_msg = prepare_message(endpoint["calllog"], method["get"], status["OK"], {"count": True})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        count = result["count"]
        if count == 0:
            print("count = 0!")
            return False

        msg, result_msg = prepare_message(endpoint["calllog"], method["get"], status["OK"], {"limit": count})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        contactId = result[0]["contactId"]
        if len(result) != count:
            print("record length mismatch!")
            return False

        for record in result:
            if record["phoneNumber"].isnumeric() == False:
                return False
            if record["date"].isnumeric() == False:
                return False
            if record["duration"].isnumeric() == False:
                return False

        msg, result_msg = prepare_message(endpoint["calllog"], method["get"], status["OK"], {"contactID": contactId})
        test = Test(self.serial, msg, result_msg)
        ret, calllog_result = test.execute()
        for calllog in calllog_result:
            if calllog["contactId"] != contactId:
                print("contactID mismatch!")
                return False

        msg, result_msg = prepare_message(endpoint["calllog"], method["del"], status["OK"], {"id": result[0]["id"]},None)
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret != True:
            return False

        msg, result_msg = prepare_message(endpoint["calllog"], method["get"], status["OK"], {"count": True})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if count != result["count"]+1:
            print("record count after delete mismatch!")
            return False

        return True
