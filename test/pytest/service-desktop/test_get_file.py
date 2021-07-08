# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status
from harness.api.developermode import PhoneModeLock
from harness.api.filesystem import get_file, FsInitGet, FsGetChunk
from harness import log
from harness.request import TransactionError
from harness.interface.defs import Status


def setPasscode(harness, flag):
    PhoneModeLock(flag).run(harness)


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_get_not_existing_file(harness):
    """
    Attempt requesting not exiting file
    """

    """
    Getting a large file may hit screen auto lock.
    We need to disable pass code for duration of test
    """
    setPasscode(harness, False)

    fileName = "Unknown.file"
    with pytest.raises(TransactionError, match=r".*" + str(Status.NotFound.value) + ".*"):
        get_file(harness, fileName, "./", "/sys/user/")


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_get_invalid_chunks(harness):
    """
    Attempts:
    requesting data with invalid chunk numbers, which are:
        - 0 as numbering start with 1
        - chunk count + 1 (or any more) as it exceeds maximum no of chunks
    requesting data with invalid rxID:
       - rxID != current rxID transfer
    """
    fileName = "MuditaOS.log"
    ret = FsInitGet("/sys/user/", fileName).run(harness)

    assert ret.fileSize != 0

    totalChunks = int(((ret.fileSize + ret.chunkSize - 1) / ret.chunkSize))
    log.info(f"totalChunks #: {totalChunks}")

    with pytest.raises(TransactionError, match=r".*" + str(Status.BadRequest.value) + ".*"):
        FsGetChunk(ret.rxID, 0).run(harness)

    with pytest.raises(TransactionError, match=r".*" + str(Status.BadRequest.value) + ".*"):
        FsGetChunk(ret.rxID, totalChunks + 1).run(harness)

    with pytest.raises(TransactionError, match=r".*" + str(Status.BadRequest.value) + ".*"):
        FsGetChunk(ret.rxID - 1, totalChunks + 1).run(harness)

    with pytest.raises(TransactionError, match=r".*" + str(Status.BadRequest.value) + ".*"):
        FsGetChunk(ret.rxID + 1, totalChunks + 1).run(harness)


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_get_file(harness):
    """
    Get file MuditaOS.log file - whole transfer
    """

    get_file(harness, "MuditaOS.log", "./")
