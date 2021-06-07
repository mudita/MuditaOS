# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status

MESSAGES_PAGE_SIZE = 4


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_messages_without_pagination(harness):
    limit = MESSAGES_PAGE_SIZE - 1
    get_body = {"category": "message", "limit": limit, "offset": 0}
    ret = harness.endpoint_request("messages", "get", get_body)
    assert ret["status"] == status["OK"]

    messages = ret["body"]["entries"]
    messages_count = len(messages)
    assert messages_count == limit
    message_types = [1, 2, 4, 8, 16, 18, 255]
    for message in messages:
        assert type(message["contactID"]) == int
        assert type(message["messageBody"]) == str
        assert type(message["messageID"]) == int
        assert type(message["messageType"]) == int
        assert type(message["createdAt"]) == int
        assert type(message["threadID"]) == int
        assert message["messageType"] in message_types


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_messages_with_pagination(harness):
    limit = MESSAGES_PAGE_SIZE + 1
    body = {"category": "message", "limit": limit, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    messages = ret["body"]["entries"]
    messages_count = len(messages)
    assert messages_count == MESSAGES_PAGE_SIZE


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_all_messages(harness):
    # get messages count
    body = {"category": "message", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    count = ret["body"]["count"]
    assert count > 0

    # get first messages page
    body = {"category": "message", "limit": count, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)

    assert ret["status"] == status["OK"]
    assert count == ret["body"]["totalCount"]

    all_messages = ret["body"]["entries"]
    if count > MESSAGES_PAGE_SIZE:

        # get rest of the messages
        while MESSAGES_PAGE_SIZE == len(ret["body"]["entries"]):

            if "nextPage" in ret["body"]:

                offset = ret["body"]["nextPage"]["offset"]
                messages_left_count = count - len(all_messages)

                body = {"category": "message", "limit": messages_left_count, "offset": offset}
                ret = harness.endpoint_request("messages", "get", body)

                assert ret["status"] == status["OK"]

                all_messages += ret["body"]["entries"]

            else:
                break

    assert count == len(all_messages)


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_messages_by_thread_id_without_pagination(harness):
    thread_id = 1
    limit = MESSAGES_PAGE_SIZE - 1
    body = {"category": "message", "threadID": thread_id, "limit": limit, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    messages = ret["body"]["entries"]
    messages_count = len(messages)
    assert messages_count == limit
    for message in ret["body"]["entries"]:
        assert message["threadID"] == thread_id


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_messages_by_thread_id_with_pagination(harness):
    thread_id = 1
    limit = MESSAGES_PAGE_SIZE + 1
    body = {"category": "message", "threadID": thread_id, "limit": limit, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    messages = ret["body"]["entries"]
    messages_count = len(messages)
    assert messages_count == MESSAGES_PAGE_SIZE
    for message in ret["body"]["entries"]:
        assert message["threadID"] == thread_id


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_all_messages_by_thread_id(harness):
    thread_id = 1
    # get messages count
    body = {"category": "message", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    messages_count = ret["body"]["count"]
    assert messages_count > 0

    # get first messages page
    body = {"category": "message", "threadID": thread_id, "limit": messages_count, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)

    assert ret["status"] == status["OK"]
    messages_by_thread_id_count = ret["body"]["totalCount"]

    all_messages = ret["body"]["entries"]
    if messages_by_thread_id_count > MESSAGES_PAGE_SIZE:

        # get rest of the messages
        while MESSAGES_PAGE_SIZE == len(ret["body"]["entries"]):

            if "nextPage" in ret["body"]:

                offset = ret["body"]["nextPage"]["offset"]
                messages_left_count = messages_by_thread_id_count - len(all_messages)

                body = {"category": "message", "threadID": thread_id, "limit": messages_left_count, "offset": offset}
                ret = harness.endpoint_request("messages", "get", body)

                assert ret["status"] == status["OK"]

                all_messages += ret["body"]["entries"]

            else:
                break

    assert messages_by_thread_id_count == len(all_messages)


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_remove_message(harness):
    # get messages count
    body = {"category": "message", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    count = ret["body"]["count"]
    assert count > 0

    # get message
    number_of_requested_messages = 1
    body = {"category": "message", "limit": number_of_requested_messages, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    messages = ret["body"]["entries"]
    messages_count = len(messages)
    assert messages_count == number_of_requested_messages

    # remove message
    sms_to_remove = messages[0]
    body = {"category": "message", "messageID": sms_to_remove["messageID"]}
    ret = harness.endpoint_request("messages", "del", body)
    assert ret["status"] == status["NoContent"]

    # get messages count again
    body = {"category": "message", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["count"] == count - 1


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_message_by_id(harness):
    message_id = 1
    get_body = {"category": "message", "messageID": message_id}
    ret = harness.endpoint_request("messages", "get", get_body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["messageID"] == message_id
