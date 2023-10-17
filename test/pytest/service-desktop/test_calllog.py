# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.skip(reason="This test should be refactored after calllog EP API final design will be ready")
@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_calllog(harness):
    # checking call log count
    body = {"count": True}
    ret = harness.endpoint_request("calllog", "get", body)
    assert ret["status"] == status["OK"]

    count = ret["body"]["count"]
    if count == 0:
        pytest.skip("No calllog entries, skipping")

    # getting the whole call log
    body = {"limit": count}
    ret = harness.endpoint_request("calllog", "get", body)
    assert ret["status"] == status["OK"]

    calllog = ret["body"]
    assert len(calllog)
    contact_id = calllog[0]["contactId"]

    for record in calllog:
        assert record["phoneNumber"].replace('+', '').isnumeric()
        assert record["date"].isnumeric()
        assert record["duration"].isnumeric()

    # getting exact call log
    body = {"contactID": contact_id}
    ret = harness.endpoint_request("calllog", "get", body)
    assert ret["status"] == status["OK"]

    for log in ret["body"]:
        assert log["contactId"] == contact_id

    # remove exact call log
    body = {"id": calllog[0]["id"]}
    ret = harness.endpoint_request("calllog", "del", body)
    assert ret["status"] == status["NoContent"]

    # getting the count again
    body = {"count": True}
    ret = harness.endpoint_request("calllog", "get", body)
    assert ret["status"] == status["OK"]

    assert count == ret["body"]["count"] + 1
