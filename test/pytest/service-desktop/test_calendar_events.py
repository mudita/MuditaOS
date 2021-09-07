# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status
import copy
import time

@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_calendar(harness):
    # add events
    add_body = {
        "calendar_events":[
            {
                "UID":"",
                "SUMMARY":"Test1",
                "DTSTART":"20200129T123600",
                "DTEND":"20200129T124600",
                "RRULE":{
                    "COUNT":"0",
                    "FREQ":"",
                    "INTERVAL":"0"
                },
                "VALARM":{
                    "ACTION":"",
                    "TRIGGER":""
                },
                "provider":{
                    "iCalUid":"",
                    "id":"",
                    "type":""
                }
            }
        ]
    }

    UIDS = []
    ret = harness.endpoint_request("events", "put", add_body)
    assert ret["status"] == status["NoContent"]
    UIDS.insert(0,ret["body"]["UID"])
    event_count = 15
    for new_event_counter  in range(2,event_count+1):
        new_event = copy.deepcopy(add_body)
        new_event["calendar_events"][0]["SUMMARY"] = "Test" + str(new_event_counter)
        ret = harness.endpoint_request("events", "put", new_event)
        assert ret["status"] == status["NoContent"]
        UIDS.insert(new_event_counter-1,ret["body"]["UID"])

    # get events limit < pageSize - pagination not used
    limit =  2
    offset=  2
    get_body = {"limit": limit, "offset": offset}
    ret = harness.endpoint_request("events", "get", get_body)
    assert ret["status"] == status["OK"]

    # get events limit > pageSize - pagination used - basic scenario
    limit = 12
    offset=  1
    get_body = {"limit": limit, "offset": offset}
    ret = harness.endpoint_request("events", "get", get_body)
    assert ret["status"] == status["OK"]


    # get events limit > pageSize - pagination used - get all events
    count = ret["body"]["totalCount"]
    batch_size = 10
    divider = int(count / batch_size)
    reminder = count % batch_size
    events = []
    for i in range(divider):
        body = {"limit": batch_size, "offset": batch_size*i}
        ret = harness.endpoint_request("events", "get", body)
        assert ret["status"] == status["OK"]
        events = events + ret["body"]["calendar_events"]
    body = {"limit": reminder, "offset": count-reminder}
    ret = harness.endpoint_request("events", "get", body)
    assert ret["status"] == status["OK"]
    events = events + ret["body"]["calendar_events"]
    events_length = len(events)
    assert events_length
    assert events_length == count

    # remove all added events - clean up
    for ev in range(0,event_count):
        del_body = {"UID": UIDS[ev]}
        ret = harness.endpoint_request("events", "del", del_body)
        assert ret["status"] == status["NoContent"]


