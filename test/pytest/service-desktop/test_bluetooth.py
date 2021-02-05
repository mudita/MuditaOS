# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status
from harness import log
import time


@pytest.mark.rt1051
@pytest.mark.service_desktop_test
def test_bluetooth(harness):

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

