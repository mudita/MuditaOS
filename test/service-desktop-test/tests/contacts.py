from defs import *
from test import *


class ContactTest:
    def run(self):
        # get contacts count
        msg, result_msg = prepare_message(endpoint["contacts"], method["get"], status["OK"],
                                          {"count": True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        count = result['count']
        if count == 0:
            print("count = 0!")
            return False

        # get all contacts
        msg, result_msg = prepare_message(endpoint["contacts"], method["get"], status["OK"],
                                          {"count": count})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        records_length = len(result)
        contact_to_update = result[0]
        if records_length != count:
            print("received record count mismatch!")
            return False

        # add contact
        msg, result_msg = prepare_message(endpoint["contacts"], method["put"], status["OK"],
                                          {"address": "6 Czeczota St.\n02600 Warsaw",
                                           "altName": "Testowy",
                                           "blocked": True,
                                           "favourite": True,
                                           "numbers": ["547623521"],
                                           "priName": "Test"}, None)
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        # again check contacts count
        msg, result_msg = prepare_message(endpoint["contacts"], method["get"], status["OK"],
                                          {"count": True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if result["count"] != count + 1:
            print("record count unchanged after add!")
            return False

        # update contact
        msg, result_msg = prepare_message(endpoint["contacts"], method["post"], status["OK"],
                                          {"address": "6 Czeczota St.\n02600 Warsaw",
                                           "altName": "Testowy2",
                                           "blocked": True,
                                           "favourite": True,
                                           "numbers": ["547623521"],
                                           "priName": "Test2",
                                           "id": contact_to_update["id"]}, None)
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        # check updated contact
        msg, result_msg = prepare_message(endpoint["contacts"], method["get"], status["OK"],
                                          {"id": contact_to_update["id"]})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        contact = {"address": "6 Czeczota St.\n02600 Warsaw",
                   "altName": "Testowy2",
                   "blocked": True,
                   "favourite": True,
                   "numbers": ["547623521"],
                   "priName": "Test2",
                   "id": contact_to_update["id"]}
        print(result)
        print(contact)
        if result != contact:
            print("updated record mismatch!")
            return False

        # get contact to remove
        msg, result_msg = prepare_message(endpoint["contacts"], method["get"], status["OK"],
                                          {"count": count + 1})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        for contact in result:
            if contact["priName"] == "Test" and contact["altName"] == "Testowy":
                id = contact["id"]
                break

        # remove contact
        msg, result_msg = prepare_message(endpoint["contacts"], method["del"], status["OK"],
                                          {"id": id}, None)
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        # again check contacts count
        msg, result_msg = prepare_message(endpoint["contacts"], method["get"], status["OK"],
                                          {"count": True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        if result["count"] != count:
            print("record count unchanged after remove!")
            return False

        return True
