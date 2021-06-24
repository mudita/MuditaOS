#!/usr/bin/env python3
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import time
import sys
import os.path
import json
import atexit

sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)))

from harness.harness import Harness
from harness.interface.defs import status
from harness.utils import Timeout
from harness.interface.error import TestError, Error
from functools import partial

# uploaded file chunk size - according to
# https://appnroll.atlassian.net/wiki/spaces/MFP/pages/656637953/Protocol+description
CHUNK_SIZE = 1024 * 16

def send_update_package(harness, package_filepath: str):
    file_size = os.path.getsize(package_filepath)
    file_name = package_filepath.split('/')[-1]

    body = {"command": "download", "fileName": file_name, "fileSize": file_size}
    ret = harness.endpoint_request("filesystem", "post", body)

    if ret["status"] != status["Accepted"]:
        print("Failed to initiate package transfer")
        print(json.dumps(ret))
        return False

    time.sleep(.1)

    print("Sending update file to the target")
    serial = harness.get_connection().get_serial()
    with open(package_filepath, 'rb') as file:
        for chunk in iter(partial(file.read, CHUNK_SIZE), b''):
            print(".", end='', flush=True)
            serial.write(chunk)

    print("")

    time.sleep(.1)

    if serial.in_waiting > 0:
        result = harness.get_connection().read(10)
        ret = json.loads(result)
        body = ret['body']

        if "status" in body:
            stat = body["status"]
            print(f"Transfer status: {stat}")

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
        raise TestError(Error.PORT_NOT_FOUND)

    harness = Harness.from_detect()

    package_filepath = str(sys.argv[1])

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
