#!/usr/bin/env python3
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import time
import sys
import os.path
import json
import atexit
import binascii
from tqdm import tqdm

sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)))

from harness.harness import Harness
from harness.interface.defs import status
from harness.interface.error import TestError, Error
from functools import partial

def send_update_package(harness, package_filepath: str):
    file_size = os.path.getsize(package_filepath)
    file_name = package_filepath.split('/')[-1]

    with open(package_filepath, 'rb') as file:
        file_data = open(package_filepath,'rb').read()
        file_crc32 = format((binascii.crc32(file_data) & 0xFFFFFFFF), '08x')

    print(f"Sending {file_name}, size {file_size}, CRC32 {file_crc32}")

    body = {"fileName" : "/sys/user/updates/" + file_name, "fileSize" : file_size, "fileCrc32" : file_crc32}
    ret = harness.endpoint_request("filesystem", "put", body)

    assert ret["status"] == status["OK"]
    assert ret["body"]["txID"] != 0

    txID      = ret["body"]["txID"]
    chunkSize = ret["body"]["chunkSize"]
    chunkNo = 1

    with open(package_filepath, 'rb') as file:
        with tqdm(total=file_size, unit='B', unit_scale=True) as pbar:
            for chunk in iter(partial(file.read, chunkSize), b''):
                data = binascii.b2a_base64(chunk).decode()
                pbar.update(chunkSize)
                body = {"txID" : txID, "chunkNo": chunkNo, "data" : data}
                ret = harness.endpoint_request("filesystem", "put", body)
                assert ret["status"] == status["OK"]
                time.sleep(.1)
                chunkNo += 1

    print("Sending complete")

    return True

def check_update_package(harness, package_filepath: str):
    body = {}
    ret = harness.endpoint_request("update", "get", body)

    if ret["status"] != status["OK"]:
       print("Failed to get package list")
       print(json.dumps(ret))
       return False

    expected_file_name = package_filepath.split('/')[-1]
    expected_file_size = os.path.getsize(package_filepath)

    file_list = ret["body"]["updateFileList"]

    if len(file_list) == 0:
        print("No update package found")
        return False

    for file in file_list:
        file_name = file["name"].split('/')[-1]

        if expected_file_name != file_name:
            print(f'File {expected_file_name} not found')
            return False

        file_size = int(file["size"])

        if expected_file_size != file_size:
            print(f'File {expected_file_name} size {file["size"]} does not match expected {expected_file_size}')
            return False

    print("Transfered package OK")
    return True

def setPasscode(harness, flag):
    body = {"phoneLockCodeEnabled": flag}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["NoContent"]

def main():
    if len(sys.argv) == 1:
        print(f'Please pass update file path as the parameter: python {sys.argv[0]} file_path ')
        raise TestError(Error.OTHER_ERROR)

    package_filepath = str(sys.argv[1])

    if (not os.path.exists(package_filepath)):
        print(f'Update file {package_filepath} not found')
        raise TestError(Error.OTHER_ERROR)

    harness = Harness.from_detect()

    atexit.register(setPasscode, harness, True)

    setPasscode(harness, False)

    time.sleep(.1)

    if not send_update_package(harness, package_filepath):
        print("Update package transfer failed")
        exit(1)

    time.sleep(.1)

    if not check_update_package(harness, package_filepath):
        print("Update package corrupted in transfer")
        exit(1)

    exit(0)

if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        print(err)
        exit(err.get_error_code())
