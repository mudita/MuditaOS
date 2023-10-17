# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest
from harness.request import TransactionError
from harness import log
from harness.rt_harness_discovery import get_harness_automatic
from harness.api.device_info import GetDeviceInfo
from harness.api.contacts import GetContactsCount, GetContactsWithOffsetAndLimit, AddContact, \
    DeleteContactById
from harness.api.backup import BackupInit
from harness.api.restore import RestoreInit
from harness.api.filesystem import FsRemoveFile

ReferenceContactIds = []
TestContactIds = []
ReferenceContacts = {}


def run_restore(harness):
    log.debug("Requesting restore")
    try:
        RestoreInit().run(harness)
    except TransactionError:
        return False
    else:
        return True


def run_backup(harness):
    log.debug("Requesting backup")
    try:
        BackupInit().run(harness)
    except TransactionError:
        return False
    else:
        return True


def remove_backup_file(harness):
    try:
        resp = GetDeviceInfo().run(harness)
        backupFilePath = resp.diag_info["backupFilePath"]
        FsRemoveFile(backupFilePath).run(harness)
    except TransactionError:
        return False
    else:
        return True


def populate_user_data(harness, contactIdList):
    try:
        initialContactsCount = GetContactsCount().run(harness).count
        log.debug(f"Initial contacts count: {initialContactsCount}")

        resp = AddContact({
            "altName": "Testowy",
            "favourite": True,
            "numbers": ["547623521"],
            "priName": "Test"}).run(harness)

        log.debug(f"Created contact id: {resp.id}")
        contactIdList.append(resp.id)

        resp = AddContact({
            "altName": "Testowy2",
            "favourite": True,
            "numbers": ["123456789"],
            "priName": "Test2"}).run(harness)

        log.debug(f"Created contact id: {resp.id}")
        contactIdList.append(resp.id)
    except TransactionError:
        return False
    else:
        return True


def clear_user_data(harness, contactIdList):
    try:
        contactsCount = GetContactsCount().run(harness).count
        for id in contactIdList:
            DeleteContactById(id).run(harness)
            log.debug(f"Deleted contact id: {id}")

        log.debug("Confirm no user data on device")

        contactsCountAfterClear = GetContactsCount().run(harness).count
        log.debug(f"Contacts after clear: {contactsCountAfterClear}")

        assert contactsCountAfterClear == contactsCount - len(contactIdList)
    except TransactionError:
        return False, -1
    else:
        return True, contactsCountAfterClear


def save_user_data(harness, contactIdList, referenceContacts):
    try:
        contactsToSave = GetContactsCount().run(harness).count
        resp = GetContactsWithOffsetAndLimit(0, contactsToSave).run(harness)

        for e in resp.entries:
            id = e["id"]
            referenceContacts[id] = e
            contactIdList.append(id)
            log.debug(f"Saved contact id: {id}")
    except TransactionError:
        return False
    else:
        return True


def compare_user_data(harness, referenceContacts, referenceContactIds, contactsCountAfterClear):
    try:
        contactsCountAfterRestore = GetContactsCount().run(harness).count

        log.debug(f"Contacts after restore: {contactsCountAfterRestore}")
        assert (len(referenceContacts) + contactsCountAfterClear) == contactsCountAfterRestore

        log.debug("Get user data from device for comparison")
        resp = GetContactsWithOffsetAndLimit(0, contactsCountAfterRestore).run(harness)

        for e in resp.entries:
            id = e["id"]
            log.debug(f"Compare contact id: {id}")
            assert referenceContacts[id] == e
            assert id in referenceContactIds
    except TransactionError:
        return False
    else:
        return True


@pytest.mark.service_desktop_test
@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.backup
def test_backup_and_restore(harness):
    log.debug("Checking/Creating user data on device")
    assert populate_user_data(harness, TestContactIds), "Failed to populate user data!"

    log.debug("Save user data from device for comparison")
    assert save_user_data(harness, ReferenceContactIds, ReferenceContacts), "Failed to save user data!"

    log.debug("Creating backup")
    assert run_backup(harness), "Failed to initialize backup!"

    timeout_seconds = 50
    harness.connection.watch_port_reboot(timeout_seconds)
    harness = get_harness_automatic(timeout_seconds)
    harness.unlock_phone()

    log.debug("Removing user data from device")
    status, contactsCountAfterClear = clear_user_data(harness, ReferenceContactIds)
    assert status, "Failed to clear user data!"

    log.debug("Requesting restore")
    assert run_restore(harness), "Failed to initialize restore process!"

    harness.connection.watch_port_reboot(timeout_seconds)
    harness = get_harness_automatic(timeout_seconds)
    harness.unlock_phone()

    log.debug("Confirm user data on device match saved data")
    assert compare_user_data(harness, ReferenceContacts, ReferenceContactIds, contactsCountAfterClear), "User data inconsistent after backup!"

    log.debug("Remove user data added by test")
    assert clear_user_data(harness, TestContactIds), "Failed to clear user data!"

    assert remove_backup_file(harness), "Failed to remove backup file!"
