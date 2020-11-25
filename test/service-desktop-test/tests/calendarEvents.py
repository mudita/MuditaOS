# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.interface.defs import endpoint, method, status

from test_api import *
from defs import *


class calendarEventsTest:
    def __init__(self, serial):
        self.serial = serial.get_serial()

    def run(self):
        # add event
        event = "BEGIN:VCALENDAR\nBEGIN:VEVENT\nSUMMARY:Testowy\nDTSTART:20200929T123611\nDTEND:20200929T124611\nBEGIN:VALARM\nTRIGGER:-P5M\nEND:VALARM\nEND:VEVENT\nEND:VCALENDAR\n"

        msg, result_msg = prepare_message(endpoint["events"], method["put"], status["OK"],
                                          {"data": event}, None)
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        #get all limited events
        msg, result_msg = prepare_message(endpoint["events"], method["get"], status["OK"],
                                          {"offset": 0,
                                           "limit":  1},
                                          {"data":event,
                                           "count": "1"})

        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        print(result)
        uid_index = result["data"].index('UID:')
        summary_index = result["data"].index('SUMMARY:')
        UID = result["data"][uid_index+4:summary_index]
        print(UID)

        if not "SUMMARY:Testowy" in result["data"]:
            return False

        if not "DTSTART:20200929T123611" in result["data"]:
            return False

        if not "DTEND:20200929T124611" in result["data"]:
            return False

        if result["count"] != "1":
            return False

        eventUpdate = "BEGIN:VCALENDAR\nBEGIN:VEVENT\nUID:"+UID+"\n"+"SUMMARY:Update\nDTSTART:20200928T123611\nDTEND:20200928T124611\nEND:VEVENT\nEND:VCALENDAR\n"
        print(eventUpdate)
        # # update event
        msg, result_msg = prepare_message(endpoint["events"], method["post"], status["OK"],
                                          {"data":eventUpdate}, None)
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        # check updated event
        msg, result_msg = prepare_message(endpoint["events"], method["get"], status["OK"],
                                          {"offset": 0,
                                           "limit":  1},
                                          {"data":eventUpdate,
                                           "count": "1"})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        print(result)

        if not "SUMMARY:Update" in result["data"]:
            return False

        if not "DTSTART:20200928T123611" in result["data"]:
            return False

        if not "DTEND:20200928T124611" in result["data"]:
            return False

        if result["count"] != "1":
            return False

        #remove event
        msg, result_msg = prepare_message(endpoint["events"], method["del"], status["OK"],
                                          {"UID": UID}, None)
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        # check events after remove
        msg, result_msg = prepare_message(endpoint["events"], method["get"], status["OK"],None,
                                          {"data":"",
                                           "count": "1"})
        test = Test(self.serial, msg, result_msg)
        ret, result = test.execute()
        if ret == False:
            return False

        return True
