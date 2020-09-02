from defs import *
from test import *


class UpdateTest:
    def run(self):
        # perform update endpoint test
        msg, result_msg = prepare_message(endpoint["update"], method["get"], status["OK"], {})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        filename = ""
        if len(result) > 0:
            filename = result

        if len(filename) > 1:
            filename = filename[0]
        msg, result_msg = prepare_message(endpoint["update"], method["post"], status["OK"], {"filename": filename})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if filename != "":
            if result["updateReady"] == True:
                return True
        else:
            if result["updateReady"] != False:
                return False

        return True
