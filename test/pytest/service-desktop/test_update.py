# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("usb_unlocked")
def test_update(harness):
    body = {}
    ret = harness.endpoint_request("update", "get", body)
    assert ret["status"] == status["OK"]

    file_list = ret["body"]["updateFileList"]

    if len(file_list) == 0:
        pytest.skip("No update files")

    for file in file_list:
        assert file["size"] != 0

    body = {"filename": file_list[0]["name"]}
    ret = harness.endpoint_request("update", "post", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["updateReady"] is True
