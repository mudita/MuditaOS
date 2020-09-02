from defs import *
from test import *


class BackupTest:
    def run(self):

        msg, result_msg = prepare_message(endpoint["backup"], method["get"], status["OK"],
                                          {"backupReady": True}, {"backupReady": False})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        msg, result_msg = prepare_message(endpoint["backup"], method["get"], status["OK"],
                                          {}, {"backupRequest": False})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        msg, result_msg = prepare_message(endpoint["backup"], method["get"], status["OK"],
                                          {"backupRequest": True}, {"backupRequest": True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        msg, result_msg = prepare_message(endpoint["backup"], method["get"], status["OK"],
                                          {"backupReady": True}, {"backupReady": True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        msg, result_msg = prepare_message(endpoint["backup"], method["post"], status["OK"],
                                          {"backupUpload": True}, {"backupUpload": False})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        return True
