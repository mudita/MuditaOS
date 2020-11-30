# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.skip("ICS format bug")
@pytest.mark.service_desktop_test
def test_calendar(harness):
    # add event
    event = "BEGIN:VCALENDAR\nBEGIN:VEVENT\nSUMMARY:Testowy\nDTSTART:20200929T123611\nDTEND:20200929T124611\nBEGIN" \
            ":VALARM\nTRIGGER:-P5M\nEND:VALARM\nEND:VEVENT\nEND:VCALENDAR\n "

    body = {"data": event}
    ret = harness.endpoint_request("events", "put", body)
    assert ret["status"] == status["OK"]

    # get all limited events
    body = {"offset": 0, "limit": 1}
    ret = harness.endpoint_request("events", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["data"] == event
    assert ret["body"]["count"] == 1

    uid_index = ret["body"]["data"].index('UID:')
    summary_index = ret["body"]["data"].index('SUMMARY:')
    UID = ret["body"]["data"][uid_index + 4:summary_index]

    assert "SUMMARY:Testowy" in ret["body"]["data"]
    assert "DTSTART:20200929T123611" in ret["body"]["data"]
    assert "DTEND:20200929T124611" in ret["body"]["data"]
    assert ret["body"]["count"] == "1"

    # update event
    event_update = "BEGIN:VCALENDAR\nBEGIN:VEVENT\nUID:" + UID + "\n" + "SUMMARY:Update\nDTSTART:20200928T123611" \
                                                                        "\nDTEND:20200928T124611\nEND:VEVENT\nEND" \
                                                                        ":VCALENDAR\n "
    body = {"data": event_update}
    ret = harness.endpoint_request("events", "post", body)
    assert ret["status"] == status["OK"]

    # get updated event
    body = {"offset": 0, "limit": 1}
    ret = harness.endpoint_request("events", "get", body)
    assert ret["status"] == status["OK"]

    assert ret["body"]["data"] == event_update
    assert ret["body"]["count"] == 1

    assert "SUMMARY:Update" in ret["body"]["data"]
    assert "DTSTART:20200928T123611" in ret["body"]["data"]
    assert "DTEND:20200928T124611" in ret["body"]["data"]
    assert ret["body"]["count"] == "1"

    # remove event
    body = {"UID": UID}
    ret = harness.endpoint_request("events", "del", body)
    assert ret["status"] == status["OK"]

    # check events after remove
    body = {"data": "", "count": "1"}
    ret = harness.endpoint_request("events", "get", body)
    assert ret["status"] == status["OK"]
