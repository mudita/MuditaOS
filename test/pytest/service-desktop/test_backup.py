# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_unlocked")
def test_backup(harness):
    body = {"backupReady": True}
    ret = harness.endpoint_request("backup", "get", body)
    assert ret["status"] == status["OK"]
    assert type(ret["body"]["backupReady"]) is bool

    body = {}
    ret = harness.endpoint_request("backup", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["backupRequest"] is False

    body = {"backupRequest": True}
    ret = harness.endpoint_request("backup", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["backupRequest"] is True

    body = {"backupReady": True}
    ret = harness.endpoint_request("backup", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["backupReady"] is True

    body = {"backupUpload": True}
    ret = harness.endpoint_request("backup", "post", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["backupUpload"] is True

