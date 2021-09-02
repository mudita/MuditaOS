#!/usr/bin/env python3
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import time
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
from harness.api.filesystem import put_file
from harness.api.developermode import PhoneModeLock

def send_update_package(harness, package_filepath: str):
    file_name = package_filepath.split('/')[-1]
    put_file(harness, file_name , "/sys/user/updates/")

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
    PhoneModeLock(flag).run(harness)

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
    send_update_package(harness, package_filepath)

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
