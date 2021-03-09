# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import time
from harness.interface.defs import status
from harness import utils, log

@pytest.mark.service_desktop_test
def test_secured_endpoint(harness):
    body = {}

    harness.lock_usb()

    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["Forbidden"]

    harness.unlock_usb()

    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["OK"]

@pytest.mark.service_desktop_test
def test_secured_paired(harness):
    body = {
        "uniqueId": "11111",
        "passcode": 3333
    }

    harness.lock_usb()

    ret = harness.endpoint_request("deviceInfo", "get", {})
    assert ret["status"] == status["Forbidden"]

    ret = harness.endpoint_request("usbSecurity", "post", body)
    assert ret["status"] == status["OK"]

    ret = harness.endpoint_request("calllog", "get", {})
    assert ret["status"] == status["OK"]

    harness.lock_usb()

    ret = harness.endpoint_request("backup", "get", {})
    assert ret["status"] == status["Forbidden"]

    ret = harness.endpoint_request("usbSecurity", "post", body)
    assert ret["status"] == status["OK"]

    ret = harness.endpoint_request("calllog", "get", {})
    assert ret["status"] == status["OK"]

    harness.unlock_usb()

@pytest.mark.service_desktop_test
def test_usb_status(harness):
    body = {
        "uniqueId": "11111",
        "passcode": 3333
    }

    harness.lock_usb()

    ret = harness.endpoint_request("usbSecurity", "get", {})
    assert ret["status"] == status["Forbidden"]

    harness.unlock_usb()

    ret = harness.endpoint_request("usbSecurity", "get", {})
    assert ret["status"] == status["OK"]

    harness.lock_usb()

    ret = harness.endpoint_request("usbSecurity", "get", {})
    assert ret["status"] == status["Forbidden"]

    ret = harness.endpoint_request("usbSecurity", "post", body)
    assert ret["status"] == status["OK"]

    ret = harness.endpoint_request("usbSecurity", "get", {})
    assert ret["status"] == status["OK"]
