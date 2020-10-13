from defs import *
from test import *


class EventsTest:
    def run(self):
        #assign records count from DB
        EXPECTED_RECORDS_LENGHT = 5

        # get all events
        # msg, result_msg = prepare_message(endpoint["events"], method["get"], status["OK"],{})
        # test = Test(msg, result_msg)
        # ret, result = test.execute()
        # records_length = result
        # #event_to_update = result[0]
        # if records_length != EXPECTED_RECORDS_LENGHT:
        #     print("received record count mismatch!")
        #     return False

        # add event
        data = {
                "data": "BEGIN:VCALENDAR\nBEGIN:VEVENT\nSUMMARY:Testowy\nDTSTAMP:20100927T123611\nDTSTART:20100927T123611\nEND:VEVENT\nEND:VCALENDAR"""
        }
        msg, result_msg = prepare_message(endpoint["events"], method["put"], status["OK"],data, None)
        print(msg)

        test = Test(msg, msg)
        ret, result = test.execute()
        if ret == False:
            return False

        # again check events count
        # msg, result_msg = prepare_message(endpoint["events"], method["get"], status["OK"],
        #                                   {"count": True})
        # test = Test(msg, result_msg)
        # ret, result = test.execute()
        # if result["count"] != EXPECTED_RECORDS_LENGHT + 1:
        #     print("record count unchanged after add!")
        #     return False

        # #get the UID of first event
        # #Use the UID to update event
        # # update event
        # msg, result_msg = prepare_message(endpoint["events"], method["post"], status["OK"],
        #                                   """
        #                                     BEGIN:VCALENDAR
        #                                     BEGIN:VEVENT
        #                                     SUMMARY:Testowy2
        #                                     DTSTAMP:20100927T123611
        #                                     DTSTART:20100927T123611
        #                                     END:VEVENT
        #                                     END:VCALENDAR
        #                                   """, None)
        # test = Test(msg, result_msg)
        # ret, result = test.execute()
        # if ret == False:
        #     return False
        #
        # # check updated event
        # msg, result_msg = prepare_message(endpoint["events"], method["get"], status["OK"],{"data": event_to_update["data"]})
        # test = Test(msg, result_msg)
        # ret, result = test.execute()
        # event = """
        #             BEGIN:VCALENDAR
        #             BEGIN:VEVENT
        #             SUMMARY:Testowy2
        #             DTSTAMP:20100927T123611
        #             DTSTART:20100927T123611
        #             END:VEVENT
        #             END:VCALENDAR
        #         """
        # if result != contact:
        #     print("updated record mismatch!")
        #     return False
        #
        # # get event to remove
        # msg, result_msg = prepare_message(endpoint["events"], method["get"], status["OK"],
        #                                   {"count": count + 1})
        # test = Test(msg, result_msg)
        # ret, result = test.execute()
        # if (event != """
        #             BEGIN:VCALENDAR
        #             BEGIN:VEVENT
        #             SUMMARY:Testowy2
        #             DTSTAMP:20100927T123611
        #             DTSTART:20100927T123611
        #             END:VEVENT
        #             END:VCALENDAR
        #         """):
        #     return False

        # remove event
        # msg, result_msg = prepare_message(endpoint["contacts"], method["del"], status["OK"],
        #                                   {"id": id}, None)
        # test = Test(msg, result_msg)
        # ret, result = test.execute()
        # if ret == False:
        #     return False
        #
        # # again check events count
        # msg, result_msg = prepare_message(endpoint["contacts"], method["get"], status["OK"],
        #                                   {"count": True})
        # test = Test(msg, result_msg)
        # ret, result = test.execute()
        # if result["count"] != count:
        #     print("record count unchanged after remove!")
        #     return False

        return True
