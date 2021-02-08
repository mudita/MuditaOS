# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
def test_messages(harness):
    # getting the messages count
    body = {"category": "message", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    count = ret["body"]["count"]
    if count == 0:
        pytest.skip("No messages entries, skipping")

    # getting all the messages
    body = {"category": "message", "limit": count, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    messages = ret["body"][1][1]  # getting entries
    messages_count = len(messages)
    assert messages_count == count

    # getting a number of messages
    number_of_requested_messages = 3
    body = {"category": "message", "limit": number_of_requested_messages, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    messages = ret["body"][1][1]  # getting entries
    messages_count = len(messages)
    assert messages_count == number_of_requested_messages

    # getting messages binded to threadID
    thread_id = 1
    body = {"category": "message", "threadID": thread_id, "limit": 10, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    for message in ret["body"][1][1]:
        assert message["threadID"] == thread_id

    # remove message
    sms_to_remove = messages[0]
    body = {"category": "message", "messageID": sms_to_remove["messageID"]}
    ret = harness.endpoint_request("messages", "del", body)
    assert ret["status"] == status["OK"]

    # getting the messages count again
    body = {"category": "message", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    assert ret["body"]["count"] == count - 1
