from defs import *
from test import *


class MessageTest:
    def run(self):
        # get messages count
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"], {"count": True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        count = result['count']
        if count == 0:
            print("count = 0!")
            return False

        # get all messages
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"], {"count": count})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        records_length = len(result)
        if records_length != count:
            print("received record count mismatch!")
            return False

        # remove message
        sms_to_remove = result[0]
        msg, result_msg = prepare_message(endpoint["messages"], method["del"], status["OK"],
                                          {"id": sms_to_remove["id"]}, None)
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return ret

        # check if the message was removed
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"], {"count": count})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        for message in result:
            if message["id"] == sms_to_remove["id"]:
                print("contact still exists!")
                return False

        # get messages binded to contactID
        contact_id = 2
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"], {"contactID": contact_id})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        for message in result:
            if message["contactID"] != contact_id:
                print("wrong contactID!")
                return False

        thread_id = 1
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"],
                                          {"threadID": thread_id, "count": 10})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        for message in result:
            if message["threadID"] != thread_id:
                print("wrong threadID!")
                return False

        return True
