#!/usr/bin/env python3
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import time
import sys
import os.path
import atexit
import base64
from tqdm import tqdm


from harness.harness import Harness
from harness.interface.defs import status
from harness.interface.error import TestError, Error
from functools import partial

def transfer_data(harness: Harness, logDir: str, fileName: str, rxID, fileSize, chunkSize):

    totalChunks = int(((fileSize + chunkSize - 1) / chunkSize))
    print(f'Transfering {fileName} to {logDir}:')

    with open(os.path.join(logDir, fileName), 'wb') as logFile:
        with tqdm(total=fileSize, unit='B', unit_scale=True) as pbar:
            for n in range(1, totalChunks + 1):
                body = {"rxID" : rxID, "chunkNo": n}
                ret = harness.endpoint_request("filesystem", "get", body)

                assert ret["status"] == status["OK"]
                pbar.update(chunkSize)
                data = ret["body"]["data"][0:-1]
                decodedData = base64.standard_b64decode(data)
                logFile.write(decodedData)

def get_log_file(harness, logDir: str):
    logDir = os.path.abspath(logDir)

    fileName = "MuditaOS.log"
    body = {"fileName" : "/sys/user/" + fileName}
    ret = harness.endpoint_request("filesystem", "get", body)

    assert ret["status"] == status["OK"]
    assert ret["body"]["fileSize"] != 0

    rxID      = ret["body"]["rxID"]
    fileSize  = ret["body"]["fileSize"]
    chunkSize = ret["body"]["chunkSize"]

    transfer_data(harness, logDir, fileName, rxID, fileSize, chunkSize)

    print("Log transfer complete")


def set_passcode(harness: Harness, flag: bool):
    body = {"phoneLockCodeEnabled": flag}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["NoContent"]

def main():
    if len(sys.argv) == 1:
        print(f'Please pass log storage directory as the parameter: \'python {sys.argv[0]} <log dir>\' ')
        raise TestError(Error.OTHER_ERROR)

    log_dir = str(sys.argv[1])

    if (not os.path.exists(log_dir)):
        print(f'Log storage directory {log_dir} not found')
        raise TestError(Error.OTHER_ERROR)

    harness = Harness.from_detect()

    atexit.register(set_passcode, harness, True)

    set_passcode(harness, False)

    get_log_file(harness, log_dir)

    exit(0)

if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        print(err)
        exit(err.get_error_code())
