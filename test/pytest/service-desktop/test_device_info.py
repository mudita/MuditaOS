# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.api.device_info import GetDeviceInfo
from harness.request import TransactionError

@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_device_info(harness):
    ret = GetDeviceInfo().run(harness)

    assert int(ret.diag_info["batteryLevel"]) <= 100
    assert ret.diag_info["batteryState"] is not None
    assert ret.diag_info["selectedSim"] is not None
    assert ret.diag_info["trayState"] is not None
    assert ret.diag_info["signalStrength"] is not None
    assert ret.diag_info["accessTechnology"] is not None
    assert ret.diag_info["networkStatus"] is not None
    assert ret.diag_info["networkOperatorName"] is not None
    assert ret.diag_info["fsTotal"] is not None
    assert ret.diag_info["fsFree"] is not None
    assert 0 < int(ret.diag_info["fsFreePercent"]) <= 100
    assert ret.diag_info["gitRevision"] is not None
    assert ret.diag_info["gitTag"] is not None
    assert ret.diag_info["gitBranch"] is not None
    #assert ret.diag_info["updateHistory"] is not None
    assert ret.diag_info["currentRTCTime"] is not None
    assert ret.diag_info["version"] is not None
    assert ret.diag_info["serialNumber"] is not None

