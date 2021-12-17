# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import logging

import pytest
import time

from harness.interface.defs import status
from harness.api.developermode import PhoneModeLock
from harness.api.security import GetPhoneLockStatus, GetPhoneLockTime, SetPhoneLockOff
from harness.request import TransactionError


class SecurityTester:
    def __init__(self, harness):
        self.harness = harness

    def confirm_phone_is_unlocked(self):
        try:
            GetPhoneLockStatus().run(self.harness)
        except TransactionError:
            return False
        else:
            return True

    def confirm_phone_is_locked(self):
        try:
            GetPhoneLockStatus().run(self.harness)
        except TransactionError as error:
            return error.status == status["Forbidden"]
        else:
            return False

    def check_if_passcode_format_validation_works(self, passcode: list):
        try:
            SetPhoneLockOff(passcode).run(self.harness)
        except TransactionError as error:
            return error.status == status["BadRequest"]
        else:
            return False

    def confirm_no_time_lock(self):
        try:
            GetPhoneLockTime().run(self.harness)
        except TransactionError as error:
            return error.status == status["UnprocessableEntity"]
        else:
            return False

    def unlock_phone(self, passcode: list = None):
        try:
            SetPhoneLockOff(passcode).run(self.harness)
            time.sleep(0.1)
        except TransactionError:
            return False
        else:
            return True


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_security_phone_unlocked(harness):
    security_tester = SecurityTester(harness)
    assert security_tester.confirm_phone_is_unlocked(), "Phone is locked, but should be unlocked!"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_locked")
def test_security_phone_locked_with_code(harness):
    security_tester = SecurityTester(harness)
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_locked")
def test_all_other_endpoints_phone_locked_with_code(harness):
    security_tester = SecurityTester(harness)
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    body = {}
    endpoints_list = ["deviceInfo",
                      "update",
                      "filesystem",
                      "backup",
                      "restore",
                      "factory",
                      "contacts",
                      "messages",
                      "calllog",
                      "events",
                      "bluetooth"]
    for endpoint_name in endpoints_list:
        ret = harness.endpoint_request(endpoint_name, "get", body)
        assert ret["status"] == status["Forbidden"]

        ret = harness.endpoint_request(endpoint_name, "post", body)
        assert ret["status"] == status["Forbidden"]

        ret = harness.endpoint_request(endpoint_name, "put", body)
        assert ret["status"] == status["Forbidden"]

        ret = harness.endpoint_request(endpoint_name, "del", body)
        assert ret["status"] == status["Forbidden"]

    body = {"getInfo": "simState"}
    ret = harness.endpoint_request("developerMode", "get", body)
    assert ret["status"] == status["OK"]


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_locked")
def test_security_phone_locked_without_code(harness):
    security_tester = SecurityTester(harness)
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    PhoneModeLock(False).run(harness)
    assert not harness.phone_mode_lock

    assert security_tester.confirm_phone_is_unlocked(), "Phone is locked, but should be unlocked!"

    PhoneModeLock(True).run(harness)
    assert harness.phone_mode_lock

    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_locked")
def test_security_unlock_phone(harness):
    """
    Attempt unlocking with wrong passcode: 1111
    Assuming 1111 is not the actual passcode :-)
    """
    passcode = [1, 1, 1, 1]
    security_tester = SecurityTester(harness)
    assert security_tester.unlock_phone(passcode), "Failed to unlock the phone!"
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    passcode = ['a', 'a', 'a', 'a']
    assert security_tester.check_if_passcode_format_validation_works(
        passcode), "Passcode with incorrect format was accepted!"
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    """
    Attempt unlocking with too short passcode: 1
    """
    passcode = [1]
    assert security_tester.check_if_passcode_format_validation_works(
        passcode), "Passcode with incorrect format was accepted!"
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    """
    Attempt unlocking with the default passcode.
    Assuming the default passcode is the actual passcode :-)
    """
    assert security_tester.unlock_phone(), "Failed to unlock the phone!"
    assert security_tester.confirm_phone_is_unlocked(), "Phone is locked, but should be unlocked!"

    harness.lock_phone()
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_locked")
def test_security_time_lock(harness):
    """
    First attempt unlocking with wrong passcode: 1111
    """
    security_tester = SecurityTester(harness)
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    passcode = [1, 1, 1, 1]
    assert security_tester.unlock_phone(passcode), "Failed to unlock the phone!"
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    assert security_tester.confirm_no_time_lock(), "Phone is time locked unexpectedly!"

    """
    Second attempt unlocking with wrong passcode: 1111
    """
    passcode = [1, 1, 1, 1]
    assert security_tester.unlock_phone(passcode), "Failed to unlock the phone!"
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    assert security_tester.confirm_no_time_lock(), "Phone is time locked unexpectedly!"

    """
    Third attempt unlocking with wrong passcode: 1111
    """
    passcode = [1, 1, 1, 1]
    assert security_tester.unlock_phone(passcode), "Failed to unlock the phone!"
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    assert time.time() < GetPhoneLockTime().run(harness).phoneLockTime <= time.time() + 30

    """
    Attempt unlocking with default (correct) passcode while phone is time locked
    """
    assert security_tester.unlock_phone(), "Failed to unlock the phone!"
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    """
    Waiting 16 seconds to make sure time lock is no longer active
    """
    time.sleep(16)

    """
    Check if time lock is no longer active
    """
    assert security_tester.confirm_no_time_lock(), "Phone is time locked unexpectedly!"

    """
    Fourth attempt unlocking with wrong passcode: 1111
    """
    passcode = [1, 1, 1, 1]
    assert security_tester.unlock_phone(passcode), "Failed to unlock the phone!"
    assert security_tester.confirm_phone_is_locked(), "Phone is unlocked, but should be locked!"

    assert time.time() < GetPhoneLockTime().run(harness).phoneLockTime <= time.time() + 30

    """
    Waiting 31 seconds to make sure time lock is no longer active
    """
    time.sleep(31)

    """
    Check if time lock is no longer active
    """
    assert security_tester.confirm_no_time_lock(), "Phone is time locked unexpectedly!"

    """
    Attempt unlocking when time lock is no longer active
    """
    assert security_tester.unlock_phone(), "Failed to unlock the phone!"
    assert security_tester.confirm_phone_is_unlocked(), "Phone is locked, but should be unlocked!"
