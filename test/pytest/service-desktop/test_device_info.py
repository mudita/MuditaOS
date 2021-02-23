# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status

@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_unlocked")
def test_device_info(harness):
    body = {}
    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["OK"]

    assert 0 < int(ret["body"]["fsFreePercent"]) <= 100
