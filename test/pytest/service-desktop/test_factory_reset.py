# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.skip(reason="This will make a factory reset")
@pytest.mark.usefixtures("usb_unlocked")
def test_factory_reset(harness):
    body = {"factoryRequest": True}
    ret = harness.endpoint_request("factory", "post", body)
    assert ret["status"] == status["OK"]
    assert ret["body"] == body
