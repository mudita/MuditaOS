# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
def test_backup(harness):
    body = {}
    ret = harness.endpoint_request("backup", "get", body)
    assert ret["status"] == status["BadRequest"]
