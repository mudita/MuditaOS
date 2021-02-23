# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status
from harness import log
import time


@pytest.mark.rt1051
@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_unlocked")
def test_bluetooth_on_off(harness):
    body = {"state": True}
    ret = harness.endpoint_request("bluetooth", "get", body)
    print(ret)
    assert ret["status"] == status["OK"]
    if ret["body"]["state"] == 0:
        log.info("BT turned off, turning on...")
        body = {"state": "on"}
        ret = harness.endpoint_request("bluetooth", "put", body)

        time.sleep(5)
        body = {"state": True}
        ret = harness.endpoint_request("bluetooth", "get", body)

        assert ret["status"] == status["OK"]

    assert ret["body"]["state"] == 1

    log.info("BT turning off...")
    body = {"state": "off"}
    ret = harness.endpoint_request("bluetooth", "put", body)

    body = {"state": True}
    ret = harness.endpoint_request("bluetooth", "get", body)
    assert ret["body"]["state"] == 0


@pytest.mark.rt1051
@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_unlocked")
def test_bluetooth_scan(harness):
    body = {"state": True}
    ret = harness.endpoint_request("bluetooth", "get", body)
    print(ret)
    assert ret["status"] == status["OK"]
    if ret["body"]["state"] == 0:
        log.info("BT turned off, turning on...")
        body = {"state": "on"}
        ret = harness.endpoint_request("bluetooth", "put", body)

        time.sleep(5)
        body = {"state": True}
        ret = harness.endpoint_request("bluetooth", "get", body)

        assert ret["status"] == status["OK"]

    assert ret["body"]["state"] == 1

    body = {"command": "scanOn"}
    ret = harness.endpoint_request("bluetooth", "put", body)
    assert ret["body"]["scan"] == "on"

    time.sleep(1)
    body = {"command": "scanOff"}
    ret = harness.endpoint_request("bluetooth", "put", body)
    assert ret["body"]["scan"] == "off"

    body = {"state": "off"}
    ret = harness.endpoint_request("bluetooth", "put", body)

    body = {"state": True}
    ret = harness.endpoint_request("bluetooth", "get", body)
    assert ret["body"]["state"] == 0
