from defs import *
from test import *


class MessageTemplateTest:
    def run(self):
        # get templates count
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"],
                                          {"template": True, "count": True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        count = result['count']
        if count == 0:
            print("count = 0!")
            return False

        # get all templates
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"],
                                          {"template": True, "count": count})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        records_length = len(result)
        if records_length != count:
            print("received record count mismatch!")
            return False

        # add template
        msg, result_msg = prepare_message(endpoint["messages"], method["put"], status["OK"],
                                          {"template": True, "text": "test template"}, None)
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        # again check templates count
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"],
                                          {"template": True, "count": True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if result["count"] != count + 1:
            print("record count unchanged after add!")
            return False

        # get template to remove
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"],
                                          {"template": True, "count": count + 1})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        for template in result:
            if template["text"] == "test template":
                id = template["id"]
                break

        # remove template
        msg, result_msg = prepare_message(endpoint["messages"], method["del"], status["OK"],
                                          {"template": True, "id": id}, None)
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        # again check templates count
        msg, result_msg = prepare_message(endpoint["messages"], method["get"], status["OK"],
                                          {"template": True, "count": True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if result["count"] != count:
            print("record count unchanged after remove!")
            return False

        return True
