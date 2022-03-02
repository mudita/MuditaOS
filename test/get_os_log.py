#!/usr/bin/env python3
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import os
import atexit

from harness.harness import Harness
from harness.request import TransactionError
from harness.interface.error import TestError, Error
from harness.interface.defs import status
from harness.api.developermode import PhoneModeLock
from harness.api.security import GetPhoneLockStatus
from harness.api.filesystem import get_log_file, get_log_file_with_path
from harness.api.device_info import *

def is_phone_locked(harness: Harness):
    try:
        GetPhoneLockStatus().run(harness)
    except TransactionError as error:
        return error.status == status["Forbidden"]
    else:
        return False

def set_passcode(harness: Harness, flag: bool):
    '''
    on exit -> restore PhoneModeLock
    '''
    PhoneModeLock(flag).run(harness)

def get_log_files(harness: Harness, fileList: DiagnosticsFileList, log_save_dir: str):
    '''
    Get list of log files and get them one by one
    '''
    ret = GetDiagnosticFilesList(fileList).run(harness)
    if not ret.files:
        print("No files to download")
        return

    for diag_file in ret.files:
        print(f'Downloading {diag_file} file:')
        get_log_file_with_path(harness, diag_file, log_save_dir)

def main():
    if len(sys.argv) == 1:
        print(
            f'Please pass log storage directory as the parameter: \'python {sys.argv[0]} <log dir>\' ')
        raise TestError(Error.OTHER_ERROR)

    log_save_dir = os.path.join(os.path.abspath(str(sys.argv[1])), '')

    if (not os.path.exists(log_save_dir)):
        print(f'Log storage directory {log_save_dir} not found')
        os.makedirs(log_save_dir, exist_ok = True)
        # raise TestError(Error.OTHER_ERROR)

    harness = Harness.from_detect()

    if is_phone_locked(harness):
        atexit.register(set_passcode, harness, True)
        set_passcode(harness, False)

    get_log_files(harness, DiagnosticsFileList.LOGS, log_save_dir)
    get_log_files(harness, DiagnosticsFileList.CRASH_DUMPS, log_save_dir)
    exit(0)


if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        print(err)
        exit(err.get_error_code())
