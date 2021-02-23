# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_unlocked")
def test_threads(harness):
    # getting all threads
    body = {"category": "thread"}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    # getting a number of threads
    number_of_requested_threads = 3
    body = {"category": "thread", "limit": number_of_requested_threads, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    threads = ret["body"]["entries"]  # getting entries
    threads_count = len(threads)
    assert threads_count == number_of_requested_threads

    # set thread as read
    body = {"category": "thread", "threadID": 1, "isUnread": False}
    ret = harness.endpoint_request("messages", "put", body)
    assert ret["status"] == status["OK"]
