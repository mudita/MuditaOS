# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import time
from harness.interface.error import Error, TestError
from harness.utils import Timeout
from harness import log
from harness.harness import Harness
from harness.api.filesystem import put_file
from harness.api.device_info import GetDeviceInfo
from harness.api.restore import RestoreInit, RestoreGetBackupList, RestoreGetState

def put_backup_file(harness, backupFile: str = ""):
    if (backupFile == ""):
        log.info("Enter path to the backup file to upload to phone:")
        backupFile = input("> ")

    if (backupFile != ""):
        backupPath = GetDeviceInfo().run(harness).diag_info["backupLocation"]
        log.debug(f'Sending backup file {backupFile} to {backupPath}')
        put_file(harness, backupFile, backupPath)
    else:
        log.debug("No backup file name provided")
        assert False

def list_restore_points(harness):
    # this requests the list of available files
    resp = RestoreGetBackupList().run(harness)

    backupList = resp.files

    # body should be an array of files
    assert isinstance(backupList, list) == True

    # chose the first entry for test if array is > 0
    if len(backupList) == 0:
        return False, []
    else:
        return True, backupList

def select_restore_point(backupList):
    availableBackups = len(backupList)

    if availableBackups > 1:
        log.info("List of restore points:")
        idx = 1
        for restore_point in backupList:
            log.info(f'{idx}: {restore_point} ')
            idx += 1

        log.info('0: Cancel')
        log.info("Select a restore point:")
        fileIdx = int(input())

        if (fileIdx == 0):
            log.debug("Test canceled by user")
            assert False
    else:
        log.info("Selecting first available backup")
        fileIdx = availableBackups

    return backupList[fileIdx - 1]

def run_restore(harness, backupList):
    restore_point = select_restore_point(backupList)
    log.debug(f"Running restore for {restore_point}")

    # this starts a restore process with a file as parameter
    resp = RestoreInit(restore_point).run(harness)

    log.debug("Restore started, waiting for results")
    time.sleep(1)  # wait for the endpoint to be ready

    # start polling for Restore status and wait for it to end
    i = 0
    try:
        with Timeout.limit(seconds=30):
            while True:
                i = i+1

                # now that we know the task ID we can poll for it's status
                resp = RestoreGetState(restore_point).run(harness)

                # Restore is still running
                if resp.state == "running":
                    log.debug("Restore is running...")

                # Restore has stopped, should be OK and finished, status is 303
                # and redirects to a location as per the documentation
                elif resp.state == "finished":
                    log.debug("Restore ended, check results")
                    break

                # Restore has stopped, should be OK and finished, status is 303
                # and redirects to a location as per the documentation
                elif resp.state == "error":
                    log.debug(f'Restore failed: {resp.reason}')
                    raise TestError(Error.TEST_FAILED)

                # wait for a moment
                log.debug("Sleeping 1s")
                time.sleep(1)
    except Timeout as e:
        log.error("Restore timeout reached")

@pytest.mark.service_desktop_test
@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.restore
def test_restore(harness: Harness):

    try:
        ret, list = list_restore_points(harness)
        if ret == False:
            log.error("No backup files on phone, run backup first")
            # put_backup_file(harness)
            assert False

        ret, list = list_restore_points(harness)
        if ret == True:
            run_restore(harness, list)
        else:
            log.error("No backup files on phone")
            raise TestError(Error.OTHER_ERROR)

    except TestError as err:
        log.debug(err)
        log.info("Restore ended, phone is restarting")
