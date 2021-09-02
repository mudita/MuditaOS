# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_threads(harness):
    # getting all threads
    body = {"category": "thread"}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    # getting a number of threads
    number_of_requested_threads = 3
    if ret["body"]["totalCount"] < number_of_requested_threads:
        number_of_requested_threads = ret["body"]["totalCount"]
    body = {"category": "thread", "limit": number_of_requested_threads, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    threads = ret["body"]["entries"]  # getting entries
    threads_count = len(threads)
    assert threads_count == number_of_requested_threads

    # Check if all fields are present and have proper type
    message_types = [1, 2, 4, 8, 16, 18, 255]
    print(ret["body"])
    for thread in ret["body"]["entries"]:
        print(thread)
        assert type(thread["contactID"]) == int
        assert type(thread["isUnread"]) == bool
        assert type(thread["lastUpdatedAt"]) == int
        assert type(thread["messageCount"]) == int
        assert type(thread["messageSnippet"]) == str
        assert type(thread["messageType"]) == int
        assert type(thread["number"]) == str
        assert type(thread["threadID"]) == int
        assert thread["messageType"] in message_types

    # set thread as read
    body = {"category": "thread", "threadID": 1, "isUnread": False}
    ret = harness.endpoint_request("messages", "put", body)
    assert ret["status"] == status["NoContent"]
