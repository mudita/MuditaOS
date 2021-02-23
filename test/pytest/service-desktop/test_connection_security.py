# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import time
from harness.interface.defs import status
from harness import utils, log

@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_locked")
def test_secured_endpoint(harness):
    body = {}

    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["Forbidden"]

    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["Forbidden"]

    ret = harness.endpoint_request("calllog", "get", body)
    assert ret["status"] == status["Forbidden"]

    ret = harness.endpoint_request("backup", "get", body)
    assert ret["status"] == status["Forbidden"]

    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["Forbidden"]

    harness.unlock_usb()

    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["OK"]

    ret = harness.endpoint_request("calllog", "get", body)
    assert ret["status"] == status["OK"]

    ret = harness.endpoint_request("backup", "get", body)
    assert ret["status"] == status["OK"]

    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]