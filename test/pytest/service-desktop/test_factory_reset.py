# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_factory_reset(harness):
    body = {"factoryRequest": True}
    ret = harness.endpoint_request("factory", "post", body)
    assert ret["status"] == status["OK"]
    assert ret["body"] == body
