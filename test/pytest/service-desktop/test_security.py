# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_security_phone_unlocked(harness):
    body = {}

    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["OK"]


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
                      "filesystemUpload",
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

    ret = harness.endpoint_request("developerMode", "get", body)
    assert ret["status"] == status["OK"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_security_phone_locked_without_code(harness):
    body = {"phoneLockCodeEnabled": False}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["OK"]

    harness.lock_phone()
    body = {}
    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["OK"]

    body = {"phoneLockCodeEnabled": True}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["OK"]

    harness.lock_phone()
    body = {}
    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["Forbidden"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_security_unlock_phone(harness):

    body = {"phoneLockCode": "1111"}
    ret = harness.endpoint_request("usbSecurity", "put", body)
    assert ret["status"] == status["Forbidden"]

    body = {"phoneLockCode": "1"}
    ret = harness.endpoint_request("usbSecurity", "put", body)
    assert ret["status"] == status["BadRequest"]

    body = {"phoneLockCode": "3333"}
    ret = harness.endpoint_request("usbSecurity", "put", body)
    assert ret["status"] == status["OK"]

    body = {}
    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["OK"]
