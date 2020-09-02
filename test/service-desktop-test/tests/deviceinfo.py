from defs import *
from test import *


class DeviceInfoTest:
    def run(self):

        msg, result_msg = prepare_message(endpoint["deviceInfo"], method["get"], status["OK"], {})
        test = Test(msg, result_msg)
        ret, result = test.execute()

        if int(result["fsFreePercent"]) == 0 or int(result["fsFreePercent"]) > 100:
            return False
        if int(result["batteryState"]) != 1:
            return False

        return True
