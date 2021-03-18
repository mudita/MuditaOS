#!/usr/bin/env python3
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import time
import sys
import os.path
import json

sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir)))

from harness import log
from harness.harness import Harness
from harness.interface.defs import key_codes, endpoint, method
from harness.utils import Timeout
from harness.interface.error import TestError, Error
from functools import partial

# uploaded file chunk size - according to
# https://appnroll.atlassian.net/wiki/spaces/MFP/pages/656637953/Protocol+description
CHUNK_SIZE = 1024 * 16

# update performing timeout
UPDATE_TIMEOUT = 90

update_status_code = {
    0: "Initial",
    1: "UpdateFileSet",
    2: "CreatingDirectories",
    3: "ExtractingFiles",
    4: "UpdatingBootloader",
    5: "ChecksumVerification",
    6: "VersionVerification",
    7: "ReadyForReset"
}


def update(harness, update_filepath: str):
    connection = harness.get_connection()
    serial = harness.get_connection().get_serial()
    file_size = os.path.getsize(update_filepath)
    filename = update_filepath.split('/')[-1]
    body = {"command": "download", "fileName": filename, "fileSize": file_size}

    ret = harness.endpoint_request("filesystemUpload", "post", body)["body"]
    if ret["status"] is not None:
        log.info(f"Update status: {update_status_code[int(ret['status'])]}")

    log.info("Downloading update file to the target")
    with open(update_filepath, 'rb') as file:
        for chunk in iter(partial(file.read, CHUNK_SIZE), b''):
            print(".", end='', flush=True)
            serial.write(chunk)
    print(" ")

    body = {"fileName": filename}
    ret = harness.endpoint_request("update", "post", body)["body"]
    if ret["fileName"] != filename and int(ret[fileSize]) != file_size:
        log.error("Upload error!")
        exit(1)
    timer = time.time()

    while True:
        if time.time() - timer > UPDATE_TIMEOUT:
            log.error("Update timeout!")
            return False
        if serial.in_waiting > 0:
            result = connection.read(10)
            ret = json.loads(result)
            body = ret['body']
            if "status" in body:
                status = body["status"]
                log.info(f"Update status: {status}")
                timer = time.time()
                if "reset" in status:
                    log.info("Update finished, wait for device reset...")
                    return True


def get_update_list(harness):
    harness.unlock_usb()
    ret = harness.endpoint_request("deviceInfo", "get", {})
    device_info = ret["body"]
    update_history = device_info["updateHistory"]
    failed_updates = 0

    if update_history is None:
        log.info("Update history clean!")
        return [None, 0]

    for update in update_history:
        if update["finishedError"] != 0 and update["finishedState"] != 6:
            failed_updates = failed_updates + 1

    log.info(f"Found {len(update_history)} update entries with {failed_updates} failed updates")
    return [update_history, failed_updates]


def main():
    if len(sys.argv) == 1:
        print(f'Please pass update file path as the parameter: python {sys.argv[0]} file_path ')
        raise TestError(Error.PORT_NOT_FOUND)

    harness = None

    with Timeout.limit(seconds=20):
        while not harness:
            try:
                harness = Harness.from_detect()
            except TestError:
                pass

    update_filename = str(sys.argv[1])
    history, fails = get_update_list(harness)

    if update(harness, update_filename):
        # wait for reboot
        harness = None
        log.info("Waiting for device to reset")
        time.sleep(5)
        # connect to the phone once again
        with Timeout.limit(seconds=90):
            while not harness:
                try:
                    harness = Harness.from_detect()
                except TestError:
                    pass

        [new_history, new_fails] = get_update_list(harness)
        if new_fails != fails or (history is None) or (history is not None and (len(new_history) != len(history) + 1)):
            log.error("Update failed!")
            exit(1)
        else:
            log.info("Update successful!")
            exit(0)
    else:
        log.error("Update error!")
        exit(1)


if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        log.error(err)
        exit(err.get_error_code())
