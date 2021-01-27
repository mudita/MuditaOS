# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status
from harness import log
import time


@pytest.mark.rt1051
@pytest.mark.service_desktop_test
def test_bluetooth(harness):

    body = {"btState": True}
    ret = harness.endpoint_request("developerMode", "put", body)
    print(ret)
    assert ret["status"] == status["OK"]
    if ret["body"]["btState"] == 0:

        log.info("BT turned off, turning on...")
        body = {"btCommand": "on"}
        ret = harness.endpoint_request("developerMode", "put", body)

        time.sleep(5)
        body = {"btState": True}
        ret = harness.endpoint_request("developerMode", "put", body)

        assert ret["status"] == status["OK"]

    assert ret["body"]["btState"] == 1



