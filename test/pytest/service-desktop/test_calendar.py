# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status
import copy


@pytest.mark.rt1051
@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_calendar(harness):
    # add events
    add_body = {
        "calendar_events":[
            {
                "UID":"",
                "SUMMARY":"Testowy",
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

    ret = harness.endpoint_request("events", "put", add_body)
    assert ret["status"] == status["OK"]

    #add second event
    second_event = copy.deepcopy(add_body)
    second_event["calendar_events"][0]["SUMMARY"] = "Testowy2"
    second_event["calendar_events"][0]["UID"] = 5
    ret = harness.endpoint_request("events", "put", second_event)
    assert ret["status"] == status["OK"]

    # get all limited events
    get_body = {"offset": 0, "limit": 1}
    ret = harness.endpoint_request("events", "get", get_body)
    assert ret["status"] == status["OK"]
    assert len(ret["body"]["calendar_events"]) == 1
    assert ret["body"]["totalCount"] == 2


    for event in ret["body"]["calendar_events"]:
        assert event["SUMMARY"] == add_body["calendar_events"][0]["SUMMARY"]
        assert event["RRULE"]["COUNT"] == add_body["calendar_events"][0]["RRULE"]["COUNT"]
        assert event["RRULE"]["FREQ"] == add_body["calendar_events"][0]["RRULE"]["FREQ"]
        assert event["RRULE"]["INTERVAL"] == add_body["calendar_events"][0]["RRULE"]["INTERVAL"]
        assert event["VALARM"]["ACTION"] == add_body["calendar_events"][0]["VALARM"]["ACTION"]
        assert event["VALARM"]["TRIGGER"] == add_body["calendar_events"][0]["VALARM"]["TRIGGER"]
        assert event["provider"]["iCalUid"] == add_body["calendar_events"][0]["provider"]["iCalUid"]
        assert event["provider"]["id"] == add_body["calendar_events"][0]["provider"]["id"]
        assert event["provider"]["type"] == add_body["calendar_events"][0]["provider"]["type"]

    # remove event
    del_body = {"UID": ret["body"]["calendar_events"][0]["UID"]}
    ret = harness.endpoint_request("events", "del", del_body)
    assert ret["status"] == status["NoContent"]

    # check events after remove
    body = {"offset": 0, "limit": 1}
    ret = harness.endpoint_request("events", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["totalCount"] == 1

    # update event
    update_body = {
        "calendar_events":[
            {
                "UID":ret["body"]["calendar_events"][0]["UID"],
                "SUMMARY":"TestowyUpdate",
                "DTSTART":"20200929T123600",
                "DTEND":"20200929T124600",
                "RRULE":{
                    "COUNT":"0",
                    "FREQ":"",
                    "INTERVAL":"0"
                },
                "VALARM":{
                    "ACTION":"",
                    "TRIGGER":"-PT5M"
                },
                "provider":{
                    "iCalUid":"",
                    "id":"",
                    "type":""
                }
            }
        ]
    }

    ret = harness.endpoint_request("events", "post", update_body)
    assert ret["status"] == status["NoContent"]

    # get updated event
    body = {"offset": 0, "limit": 1}
    ret = harness.endpoint_request("events", "get", body)
    assert ret["status"] == status["OK"]

    for event in ret["body"]["calendar_events"]:
        assert event["SUMMARY"] == update_body["calendar_events"][0]["SUMMARY"]
        assert event["RRULE"]["COUNT"] == update_body["calendar_events"][0]["RRULE"]["COUNT"]
        assert event["RRULE"]["FREQ"] == update_body["calendar_events"][0]["RRULE"]["FREQ"]
        assert event["RRULE"]["INTERVAL"] == update_body["calendar_events"][0]["RRULE"]["INTERVAL"]
        assert event["VALARM"]["ACTION"] == update_body["calendar_events"][0]["VALARM"]["ACTION"]
        assert event["VALARM"]["TRIGGER"] == update_body["calendar_events"][0]["VALARM"]["TRIGGER"]
        assert event["provider"]["iCalUid"] == update_body["calendar_events"][0]["provider"]["iCalUid"]
        assert event["provider"]["id"] == update_body["calendar_events"][0]["provider"]["id"]
        assert event["provider"]["type"] == update_body["calendar_events"][0]["provider"]["type"]

    assert ret["body"]["totalCount"] == 1



    # remove second event
    body = {"offset": 0, "limit": 1}
    ret = harness.endpoint_request("events", "get", body)
    assert ret["status"] == status["OK"]

    del_body = {"UID": ret["body"]["calendar_events"][0]["UID"]}
    ret = harness.endpoint_request("events", "del", del_body)
    assert ret["status"] == status["NoContent"]

    # check events after remove
    body = {"offset": 0, "limit": 1}
    ret = harness.endpoint_request("events", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["totalCount"] == 0



    # INVALID FORMAT
    add_body_invalid = copy.deepcopy(add_body)
    update_body_invalid = copy.deepcopy(update_body)

    # add events invalid date and time formats
    # wrong month
    add_body_invalid["calendar_events"][0]["DTSTART"] = "20201329T123600"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # wrong day
    add_body_invalid["calendar_events"][0]["DTSTART"] = "20201132T123600"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # wrong year
    add_body_invalid["calendar_events"][0]["DTEND"] = "50201120T123600"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # wrong hour
    add_body_invalid["calendar_events"][0]["DTSTART"] = "20201120T253600"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # wrong minute
    add_body_invalid["calendar_events"][0]["DTEND"] = "20201122T126300"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # wrong second
    add_body_invalid["calendar_events"][0]["DTSTART"] = "20201122T1200OO"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    #wrong separator
    add_body_invalid["calendar_events"][0]["DTSTART"] = "20201122R120059"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # wrong length
    add_body_invalid["calendar_events"][0]["DTSTART"] = "20201122T12550000"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    add_body_invalid["calendar_events"][0]["DTEND"] = "201122T125500"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # Summary invalid
    add_body_invalid["calendar_events"][0]["SUMMARY"] = ""
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]

    # Alarm invalid
    #trigger
    add_body_invalid["calendar_events"][0]["VALARM"]["TRIGGER"] = "-PTM"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    add_body_invalid["calendar_events"][0]["VALARM"]["TRIGGER"] = "-PT4T"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    add_body_invalid["calendar_events"][0]["VALARM"]["TRIGGER"] = "-PT63M"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # Recurrency Rule invalid
    # Freq
    add_body_invalid["calendar_events"][0]["RRULE"]["FREQ"] = "DAIL"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # Count
    add_body_invalid["calendar_events"][0]["RRULE"]["COUNT"] = ""
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # interval
    add_body_invalid["calendar_events"][0]["RRULE"]["COUNT"] = ""
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)

    # UID invalid (UID should be empty in add event request)
    add_body_invalid["calendar_events"][0]["UID"] = "20201122T125500-63"
    ret = harness.endpoint_request("events", "put", add_body_invalid)
    assert ret["status"] == status["BadRequest"]
    add_body_invalid = copy.deepcopy(add_body)


    # UPDATE INVALID
    # UID invalid
    update_body_invalid["calendar_events"][0]["UID"] = ""
    ret = harness.endpoint_request("events", "post", update_body_invalid)
    assert ret["status"] == status["BadRequest"]
    update_body_invalid = copy.deepcopy(update_body_invalid)

    update_body_invalid["calendar_events"][0]["UID"] = "43526gsdgsa322"
    ret = harness.endpoint_request("events", "post", update_body_invalid)
    assert ret["status"] == status["BadRequest"]
    update_body_invalid = copy.deepcopy(update_body_invalid)

    # REMOVE INVALID
    # It is a valid UID, but not exist in any events in DB
    # (handle it in db)
    # del_body = {"UID": "20151122T125500-63"}
    #
    # ret = harness.endpoint_request("events", "post", del_body)
    # assert ret["status"] == status["InternalServerError"]

    # UID invalid
    del_body = {"UID": "20201122T125500-dadsadsada"}
    ret = harness.endpoint_request("events", "del", del_body)
    assert ret["status"] == status["BadRequest"]

    del_body = {"UID": "201122T125500-63"}
    ret = harness.endpoint_request("events", "del", del_body)
    assert ret["status"] == status["BadRequest"]

