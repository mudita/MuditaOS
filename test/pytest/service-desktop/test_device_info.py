# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_device_info(harness):
    body = {}
    ret = harness.endpoint_request("deviceInfo", "get", body)
    assert ret["status"] == status["OK"]

    assert int(ret["body"]["batteryLevel"]) <= 100
    assert ret["body"]["batteryState"] is not None
    assert ret["body"]["selectedSim"] is not None
    assert ret["body"]["trayState"] is not None
    assert ret["body"]["signalStrength"] is not None
    assert ret["body"]["accessTechnology"] is not None
    assert ret["body"]["networkStatus"] is not None
    assert ret["body"]["networkOperatorName"] is not None
    assert ret["body"]["fsTotal"] is not None
    assert ret["body"]["fsFree"] is not None
    assert 0 < int(ret["body"]["fsFreePercent"]) <= 100
    assert ret["body"]["gitRevision"] is not None
    assert ret["body"]["gitTag"] is not None
    assert ret["body"]["gitBranch"] is not None
    #assert ret["body"]["updateHistory"] is not None
    assert ret["body"]["currentRTCTime"] is not None
    assert ret["body"]["version"] is not None
    assert ret["body"]["serialNumber"] is not None
    assert ret["body"]["caseColour"] is not None

