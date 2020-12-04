# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
def test_messages(harness):
    # getting the messages count
    body = {"count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    count = ret["body"]["count"]
    if count == 0:
        pytest.skip("No contacts entries, skipping")

    # getting all the messages
    body = {"count": count}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    messages = ret["body"]
    messages_count = len(messages)
    assert messages_count == count

    # remove message
    sms_to_remove = messages[0]
    body = {"id": sms_to_remove["id"]}
    ret = harness.endpoint_request("messages", "del", body)
    assert ret["status"] == status["OK"]

    # getting the messages count again
    body = {"count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    assert ret["body"]["count"] == count - 1

    # getting messages binded to contactID
    contact_id = 2  # in test dataset this one has some messages
    body = {"contactID": contact_id}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    for message in ret["body"]:
        assert message["contactID"] == contact_id

    # getting messages binded to threadID
    thread_id = 1
    body = {"threadID": thread_id, "count": 10}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    for message in ret["body"]:
        assert message["threadID"] == thread_id
