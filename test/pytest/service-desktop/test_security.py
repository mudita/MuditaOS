# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
import time

from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_security_phone_unlocked(harness):
    body = {}

    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["NoContent"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_locked")
def test_security_phone_locked_with_code(harness):
    body = {}

    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["Forbidden"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_locked")
def test_all_other_endpoints_phone_locked_with_code(harness):
    body = {}
    endpoints_list = ["deviceInfo",
                      "update",
                      "filesystem",
                      "backup",
                      "restore",
                      "factory",
                      "contacts",
                      "messages",
                      "calllog",
                      "events",
                      "bluetooth"]
    for endpoint_name in endpoints_list:
        print(endpoint_name)
        ret = harness.endpoint_request(endpoint_name, "get", body)
        assert ret["status"] == status["Forbidden"]

        ret = harness.endpoint_request(endpoint_name, "post", body)
        assert ret["status"] == status["Forbidden"]

        ret = harness.endpoint_request(endpoint_name, "put", body)
        assert ret["status"] == status["Forbidden"]

        ret = harness.endpoint_request(endpoint_name, "del", body)
        assert ret["status"] == status["Forbidden"]

    body = {"getInfo": "simState"}
    ret = harness.endpoint_request("developerMode", "get", body)
    assert ret["status"] == status["OK"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_locked")
def test_security_phone_locked_without_code(harness):

    body = {"phoneLockCodeEnabled": False}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["NoContent"]

    time.sleep(.1)

    body = {}
    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["NoContent"]

    body = {"phoneLockCodeEnabled": True}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["NoContent"]

    harness.lock_phone()
    time.sleep(.1)

    body = {}
    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["Forbidden"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_locked")
def test_security_unlock_phone(harness):

    """
    Attempt unlocking with wrong passcode: 1111
    Assuming 1111 is not the actual passcode :-)
    """
    body = {"phoneLockCode": [1, 1, 1, 1]}
    ret = harness.endpoint_request("usbSecurity", "put", body)
    assert ret["status"] == status["NoContent"]

    time.sleep(.1)

    body = {}
    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["Forbidden"]

    time.sleep(.1)

    body = {"phoneLockCode": ['a', 'a', 'a', 'a']}
    ret = harness.endpoint_request("usbSecurity", "put", body)
    assert ret["status"] == status["BadRequest"]

    time.sleep(.1)

    body = {}
    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["Forbidden"]

    time.sleep(.1)

    """
    Attempt unlocking with too short passcode: 1
    """
    body = {"phoneLockCode": [1]}
    ret = harness.endpoint_request("usbSecurity", "put", body)
    assert ret["status"] == status["BadRequest"]

    time.sleep(.1)

    body = {}
    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["Forbidden"]

    time.sleep(.1)

    """
    Attempt unlocking with correct passcode: 3333
    Assuming 3333 is the actual passcode :-)
    """
    body = {"phoneLockCode": [3, 3, 3, 3]}
    ret = harness.endpoint_request("usbSecurity", "put", body)
    assert ret["status"] == status["NoContent"]

    time.sleep(.1)

    body = {}
    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["OK"]
