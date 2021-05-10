# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_unlocked")
def test_security_usb_unlocked(harness):
    body = {}

    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["OK"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_locked")
def test_security_usb_locked(harness):
    body = {}

    ret = harness.endpoint_request("usbSecurity", "get", body)
    assert ret["status"] == status["Forbidden"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_locked")
def test_all_other_endpoints_usb_locked(harness):
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

    harness.unlock_usb()
