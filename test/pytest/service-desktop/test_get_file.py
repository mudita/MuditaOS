# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import base64
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
    print("totalChunks #: " + str(totalChunks))

    body = {"rxID" : rxID, "chunkNo": 0}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["BadRequest"]

    body = {"rxID" : rxID, "chunkNo": int(totalChunks + 1)}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["BadRequest"]


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
    assert ret["body"]["fileSize"] != 0

    rxID      = ret["body"]["rxID"]
    fileSize  = ret["body"]["fileSize"]
    chunkSize = ret["body"]["chunkSize"]

    totalChunks = int(((fileSize + chunkSize - 1) / chunkSize))
    print("totalChunks #: " + str(totalChunks))

    data = ""

    for n in range(1, totalChunks + 1):
        body = {"rxID" : rxID, "chunkNo": n}
        ret = harness.endpoint_request("filesystem", "get", body)

        assert ret["status"] == status["OK"]

        data += ret["body"]["data"][0:-1] # Skiping null char at end of chunk

    file_64 = open(fileName + ".base64" , 'w')
    file_64.write(data)

    file_64_decode = base64.standard_b64decode(data)
    file_result = open(fileName, 'wb')
    file_result.write(file_64_decode)


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
    assert ret["body"]["fileSize"] != 0

    rxID      = ret["body"]["rxID"]
    fileSize  = ret["body"]["fileSize"]
    chunkSize = ret["body"]["chunkSize"]

    totalChunks = int(((fileSize + chunkSize - 1) / chunkSize))
    print("totalChunks #: " + str(totalChunks))

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
