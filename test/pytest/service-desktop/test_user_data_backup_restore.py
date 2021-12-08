# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import time
from harness import log
from harness.utils import Timeout
from harness.interface.error import Error, TestError
from harness.rt_harness_discovery import get_harness_automatic
from harness.api.device_info import GetDeviceInfo
from harness.api.filesystem import get_log_file_with_path
from harness.api.contacts import *
from harness.api.backup import BackupInit, BackupGetState
from harness.api.restore import RestoreInit, RestoreGetBackupList, RestoreGetState
from harness.api.filesystem import FsRemoveFile

ReferenceContactIds = []
TestContactIds = []

addedContacts = 0

ReferenceContacts = {}

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

def run_backup(harness):
    log.debug("Requesting backup")

    resp = BackupInit().run(harness)

    assert resp.taskId != ""
    taskId = resp.taskId
    # in response we get a task ID and status 200
    log.debug("Backup started, waiting for results")

    time.sleep(1)  # wait for the endpoint to be ready

    # start polling for backup status and wait for it to end
    i = 0
    try:
        with Timeout.limit(seconds=30):
            while True:
                i = i+1

                # now that we know the task ID we can poll for it's status
                # body = { "id": response.taskId }
                resp = BackupGetState(taskId).run(harness)

                # Backup is still running
                if resp.state == "running":
                    log.debug("Backup is running...")

                # Backup has stopped, should be OK and finished, status is 303
                # and redirects to a location as per the documentation
                elif resp.state == "finished":
                    log.debug("Backup ended, checking results")

                    resp = GetDeviceInfo().run(harness)
                    bkpPath = resp.diag_info["backupLocation"]

                    # Retrieving backup file from phone
                    p = bkpPath + "/" + taskId
                    log.debug(f'Backup file path: {p}')
                    # get_log_file_with_path(harness, p, "./")
                    break

                # Backup ended with error
                elif resp.state == "error":
                    log.debug(f'Backup failed: {resp.reason}')
                    raise TestError(Error.TEST_FAILED)

                # wait for a moment
                log.debug("Sleeping 1s")
                time.sleep(1)
    except Timeout as e:
        log.error("Backup timeout reached")
        raise TestError(Error.OtherError)

def remove_stalled_backups(harness):
    resp = GetDeviceInfo().run(harness)
    bkpPath = resp.diag_info["backupLocation"]

    # Removing stalled backup file from phone
    ret, list = list_restore_points(harness)
    if ret == True:
        for b in list:
            FsRemoveFile(bkpPath + "/" + b).run(harness)

def perform_restore(harness):
    ret, list = list_restore_points(harness)
    if ret == True:
        try:
            run_restore(harness, list)

        except TestError as err:
            log.debug("Restore done, rebooting")
    else:
        log.error("No backup files on phone")
        raise TestError(Error.OTHER_ERROR)

def populate_user_data(harness, contactIdList):
    initialContactsCount = GetContactsCount().run(harness).count
    log.debug(f"Initial contacts count: {initialContactsCount}")
    addedContacts = 0

    resp = AddContact({
        "altName": "Testowy",
        "favourite": True,
        "numbers": ["547623521"],
        "priName": "Test"}).run(harness)

    log.debug(f"Created contact id: {resp.id}")
    contactIdList.append(resp.id)
    addedContacts += 1

    resp = AddContact({
        "altName": "Testowy2",
        "favourite": True,
        "numbers": ["123456789"],
        "priName": "Test2"}).run(harness)

    log.debug(f"Created contact id: {resp.id}")
    contactIdList.append(resp.id)
    addedContacts += 1

    return addedContacts

def clear_user_data(harness, contactIdList):
    contactsCount = GetContactsCount().run(harness).count
    for id in contactIdList:
        DeleteContactById(id).run(harness)
        log.debug(f"Deleted contact id: {id}")

    log.debug("Confirm no user data on device")

    contactsCountAfterClear = GetContactsCount().run(harness).count
    log.debug(f"Contacts after clear: {contactsCountAfterClear}")

    assert contactsCountAfterClear == contactsCount - len(contactIdList)

def save_user_data(harness, contactIdList, referenceContacts):
    contactsToSave = GetContactsCount().run(harness).count
    resp = GetContactsWithOffsetAndLimit(0, contactsToSave).run(harness)

    for e in resp.entries:
        id = e["id"]
        referenceContacts[id] = e
        contactIdList.append(id)
        log.debug(f"Saved contact id: {id}")


def compare_user_data(harness, referenceContacts, referenceContactIds):
    contactsCountAfterRestore = GetContactsCount().run(harness).count

    log.debug(f"Contacts after restore: {contactsCountAfterRestore}")
    assert len(referenceContacts) == contactsCountAfterRestore

    log.debug("Get user data from device for comparison")
    resp = GetContactsWithOffsetAndLimit(0, contactsCountAfterRestore).run(harness)

    for e in resp.entries:
        id = e["id"]
        log.debug(f"Compare contact id: {id}")
        assert referenceContacts[id] == e
        assert id in referenceContactIds


@pytest.mark.service_desktop_test
@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.backup
def test_backup_and_restore(harness):
    log.debug("Checking/Creating user data on device")
    addedContacts = populate_user_data(harness, TestContactIds)

    log.debug("Save user data from device for comparison")
    save_user_data(harness, ReferenceContactIds, ReferenceContacts)

    remove_stalled_backups(harness)

    log.debug("Creating backup")
    run_backup(harness)

    log.debug("Removing user data from device")
    clear_user_data(harness, ReferenceContactIds)

    log.debug("Requesting restore")
    perform_restore(harness)

    harness.connection.watch_port_reboot(20)
    harness = get_harness_automatic(20)
    harness.unlock_phone()

    log.debug("Confirm user data on device match saved data")
    compare_user_data(harness, ReferenceContacts, ReferenceContactIds)

    log.debug("Remove user data added by test")
    clear_user_data(harness, TestContactIds)
