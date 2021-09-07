# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import filecmp


from harness.api.filesystem import get_file, put_file

fileName = "lorem-ipsum.txt"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_send_file(harness):
    """
    Attempt requesting and sending file to Pure
    """
    put_file(harness, fileName, "/sys/user/")


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_get_file_back(harness):
    """
    Attempt requesting and transfering file data
    """
    loadedFile = "./lorem-ipsum-2.txt"

    get_file(harness, fileName, "./", "/sys/user/", loadedFile)

    assert filecmp.cmp(fileName, loadedFile, shallow=True)
