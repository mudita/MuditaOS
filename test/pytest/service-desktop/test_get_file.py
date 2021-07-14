# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import binascii
from harness import log
from harness.interface.defs import status

def setPasscode(harness, flag):
    body = {"phoneLockCodeEnabled": flag}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["NoContent"]

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
    body = {"fileName" : "/sys/user/" + fileName}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["NotFound"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_get_invalid_chunks(harness):
    """
    Attempt requesting data with invalid chunk numbers
    """
    fileName = "MuditaOS.log"
    body = {"fileName" : "/sys/user/" + fileName}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["OK"]
    assert ret["body"]["fileSize"] != 0

    rxID      = ret["body"]["rxID"]
    fileSize  = ret["body"]["fileSize"]
    chunkSize = ret["body"]["chunkSize"]

    totalChunks = int(((fileSize + chunkSize - 1) / chunkSize))
    log.info("totalChunks #: " + str(totalChunks))

    body = {"rxID" : rxID, "chunkNo": 0}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["BadRequest"]

    body = {"rxID" : rxID, "chunkNo": int(totalChunks + 1)}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["BadRequest"]

def validate_response_body(body):
    assert body["rxID"] != 0
    assert body["fileSize"] != 0
    assert body["chunkSize"] != 0
    assert body["fileCrc32"] != ""


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_get_file(harness):
    """
    Attempt requesting and transfering file data
    """
    fileName = "MuditaOS.log"
    body = {"fileName" : "/sys/user/" + fileName}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["OK"]

    validate_response_body(ret["body"])

    rxID      = ret["body"]["rxID"]
    fileSize  = ret["body"]["fileSize"]
    chunkSize = ret["body"]["chunkSize"]
    expectedCrc32 = ret["body"]["fileCrc32"]

    totalChunks = int(((fileSize + chunkSize - 1) / chunkSize))
    log.info("totalChunks #: " + str(totalChunks))
    log.info("Expected file CRC32: " + expectedCrc32)

    data = ""

    for n in range(1, totalChunks + 1):
        body = {"rxID" : rxID, "chunkNo": n}
        ret = harness.endpoint_request("filesystem", "get", body)

        assert ret["status"] == status["OK"]

        data += ret["body"]["data"][0:-1] # Skiping null char at end of chunk

    file_64 = open(fileName + ".base64" , 'w')
    file_64.write(data)

    file_64_decode = binascii.a2b_base64(data)
    file_result = open(fileName, 'wb')
    file_result.write(file_64_decode)

    actualCrc32 = format((binascii.crc32(file_64_decode) & 0xFFFFFFFF), '08x')
    log.info("Actual file CRC32: " + actualCrc32)
    assert expectedCrc32 == actualCrc32


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_get_invalid_rxID(harness):
    """
    Attempt requesting data with invalid rxIDs
    """
    fileName = "MuditaOS.log"
    body = {"fileName" : "/sys/user/" + fileName}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["OK"]

    validate_response_body(ret["body"])

    rxID      = ret["body"]["rxID"]
    fileSize  = ret["body"]["fileSize"]
    chunkSize = ret["body"]["chunkSize"]
    expectedCrc32 = ret["body"]["fileCrc32"]

    totalChunks = int(((fileSize + chunkSize - 1) / chunkSize))
    log.info("totalChunks #: " + str(totalChunks))

    body = {"rxID" : int(rxID - 1), "chunkNo": 1}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["BadRequest"]

    body = {"rxID" : int(rxID + 1), "chunkNo": 1}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["BadRequest"]

    """
    Getting a large file may hit screen auto lock.
    We need to disable pass code for duration of test
    """
    setPasscode(harness, True)
