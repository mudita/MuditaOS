# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import filecmp

from harness.api.developermode import PhoneModeLock
from harness.api.filesystem import get_file, put_file, FsInitGet, FsGetChunk, FsRemoveFile, FsRenameFile
from harness import log
from harness.request import TransactionError
from harness.interface.defs import Status


def setPasscode(harness, flag):
    PhoneModeLock(flag).run(harness)


testFileName = "lorem-ipsum.txt"
osLogFileName = "MuditaOS.log"

sysUserPath = "/sys/user"
sysUserLogsPath = sysUserPath + "/logs"

@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_send_file(harness):
    """
    Attempt requesting and sending file to Pure
    """
    put_file(harness, testFileName, sysUserPath)


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_get_file_back(harness):
    """
    Attempt requesting and transfering file data
    """
    loadedFile = "./lorem-ipsum-2.txt"

    get_file(harness, testFileName, "./", sysUserPath, loadedFile)

    assert filecmp.cmp(testFileName, loadedFile, shallow=True)

@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_rename_file(harness):
    """
    Attempt renaming a file and confirm by getting it back and comparing with original
    """
    sourceFile = "/sys/user/" + testFileName
    renamedFile = "renamed_" + testFileName
    destFile = "/sys/user/" + renamedFile

    FsRenameFile(sourceFile, destFile).run(harness)

    get_file(harness, renamedFile, "./", sysUserPath)

    assert filecmp.cmp(testFileName, renamedFile, shallow=True)

@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_delete_files(harness):
    """
    Attempt deleting a file and confirm removal by attempting to get it back
    """
    fileToRemove = sysUserPath + "/" + "renamed_" +  testFileName
    FsRemoveFile(fileToRemove).run(harness)

    with pytest.raises(TransactionError, match=r".*" + str(Status.NotFound.value) + ".*"):
        get_file(harness, testFileName, "./", sysUserPath)


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

    testFileName = "Unknown.file"
    with pytest.raises(TransactionError, match=r".*" + str(Status.NotFound.value) + ".*"):
        get_file(harness, testFileName, "./", sysUserLogsPath)


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
    ret = FsInitGet(sysUserLogsPath, osLogFileName).run(harness)

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
    get_file(harness, osLogFileName, "./", sysUserLogsPath)
