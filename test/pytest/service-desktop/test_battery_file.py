# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.rt1051
@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_battery_file(harness):
    body = {"command" : "checkFile" , "fileName" : "/sys/user/batteryFuelGaugeConfig.cfg"}
    ret = harness.endpoint_request("filesystem", "post", body)

    assert ret["status"] == status["OK"]
    assert ret["body"]["fileExists"] == True
