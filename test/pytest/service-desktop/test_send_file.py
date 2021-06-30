# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import binascii
import os.path

from harness.interface.defs import status
from functools import partial

@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_send_file(harness):
    """
    Attempt requesting and sending file to Pure
    """
    fileName = "lorem-ipsum.txt"
    filePath = os.getcwd() + "/" + fileName

    with open(filePath, 'rb') as file:
        file_data = file.read()
        fileCrc32 = format((binascii.crc32(file_data) & 0xFFFFFFFF), '08x')
        print("fileCrc32: " + fileCrc32)

    fileSize = os.path.getsize(fileName)

    body = {"fileName" : "/sys/user/" + fileName, "fileSize" : fileSize, "fileCrc32" : fileCrc32}
    ret = harness.endpoint_request("filesystem", "put", body)

    assert ret["status"] == status["OK"]
    assert ret["body"]["txID"] != 0

    txID      = ret["body"]["txID"]
    chunkSize = ret["body"]["chunkSize"]
    print("chunkSize: " + str(chunkSize))

    totalChunks = int(((fileSize + chunkSize - 1) / chunkSize))
    print("totalChunks #: " + str(totalChunks))
    chunkNo = 1

    with open(filePath, 'rb') as file:
        for chunk in iter(partial(file.read, chunkSize), b''):
            data = binascii.b2a_base64(chunk).decode()

            body = {"txID" : txID, "chunkNo": chunkNo, "data" : data}
            ret = harness.endpoint_request("filesystem", "put", body)
            assert ret["status"] == status["OK"]
            chunkNo += 1


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_get_file_back(harness):
    """
    Attempt requesting and transfering file data
    """
    fileName = "lorem-ipsum.txt"
    body = {"fileName" : "/sys/user/" + fileName}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["OK"]
    assert ret["body"]["fileSize"] != 0

    rxID      = ret["body"]["rxID"]
    fileSize  = ret["body"]["fileSize"]
    chunkSize = ret["body"]["chunkSize"]
    expectedCrc32 = ret["body"]["fileCrc32"]

    totalChunks = int(((fileSize + chunkSize - 1) / chunkSize))
    print("totalChunks #: " + str(totalChunks))
    print("Expected file CRC32: " + expectedCrc32)

    data = ""

    for n in range(1, totalChunks + 1):
        body = {"rxID" : rxID, "chunkNo": n}
        ret = harness.endpoint_request("filesystem", "get", body)

        assert ret["status"] == status["OK"]
        data += ret["body"]["data"][0:-1] # Skiping null char at end of chunk

    file_64_decode = binascii.a2b_base64(data)
    actualCrc32 = format((binascii.crc32(file_64_decode) & 0xFFFFFFFF), '08x')
    print("Actual file CRC32: " + actualCrc32)
    assert expectedCrc32 == actualCrc32

