# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import os
import pytest
from harness import log
from harness.interface.defs import Method, Endpoint
from harness.request import Transaction, Request,TransactionError

from harness.harness import Harness
import binascii


def send_file(harness: Harness, file: str, where: str) -> Transaction:

    fileSize = os.path.getsize(file)


    with open(file, 'rb') as l_file:
        file_data = l_file.read()
        fileCrc32 = format((binascii.crc32(file_data) & 0xFFFFFFFF), '08x')

    body = {"fileName": where + file,
            "fileSize": fileSize, "fileCrc32": fileCrc32}

    log.debug(f"sending {body}")

    ret = harness.request(Endpoint.FILESYSTEM, Method.PUT, body)

    log.debug(f"response {ret.response}")
    assert ret.response.body["txID"] != 0

    txID = ret.response.body["txID"]
    chunkSize = ret.response.body["chunkSize"]

    chunkNo = 1
    sent = 0

    from functools import partial
    import base64
    with open(file, 'rb') as l_file:
        for chunk in iter(partial(l_file.read, chunkSize), b''):
            data = base64.standard_b64encode(chunk).decode()

            body = {"txID": txID, "chunkNo": chunkNo, "data": data}
            ret = harness.request(Endpoint.FILESYSTEM, Method.PUT, body)
            chunkNo += 1

            sent += len(chunk)
            percent = (sent / fileSize)*100
            time = ret.send_time + ret.read_time
            Mbps = (len(data)*8)/(time*1024*1024)

            log.info(f"-> {percent:0.2f}% in {time:0.3f}s {Mbps:0.3f}Mbps")

    return ret


@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_update(harness: Harness):
    filename = "update.tar"
    try:
        harness.request(Endpoint.DEVELOPERMODE, Method.PUT, {"phoneLockCodeEnabled": False})
        send_file(harness, filename, "/sys/user/")
        harness.request(Endpoint.DEVELOPERMODE, Method.PUT, {"phoneLockCodeEnabled": True})
        harness.request(Endpoint.DEVELOPERMODE, Method.POST, {"update": True, "reboot": True})
    except TransactionError as err:
        log.error(f"{err.message} Status: {err.status}")
    log.info("update done!")
