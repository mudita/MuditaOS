# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status

@pytest.mark.skip("rkubiak01 is fixing this issue")
@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_unlocked")
def test_backup(harness):
    body = { "request": True }
    ret = harness.endpoint_request("backup", "get", body)
    assert ret["status"] == status["BadRequest"]
