# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest
from harness.request import TransactionError
from harness.api.sync import RequestSyncPackagePreparation, GetSyncPackagePreparationState
from harness.api.filesystem import get_file, FsRemoveFile, FsRenameFile, FsListFiles

SYNC_PACKAGE_PATH = "/sys/user/sync/"

class SyncTester:
    def __init__(self, harness):
        self.harness = harness

    def request_sync_package_preparation(self):
        try:
            task_id = RequestSyncPackagePreparation().run(self.harness).taskId
        except TransactionError:
            return False
        else:
            return True, task_id

    def get_sync_package_preparation_state(self, id):
        try:
            state = GetSyncPackagePreparationState(id).run(self.harness).state
        except TransactionError:
            return False
        else:
            return True, state

    def get_sync_package(self, file_name):
        try:
            get_file(self.harness, file_name, "./", SYNC_PACKAGE_PATH, file_name)
        except TransactionError:
            return False
        else:
            return True

    def delete_sync_package(self, file_name):
        try:
            FsRemoveFile(SYNC_PACKAGE_PATH + file_name).run(self.harness)
        except TransactionError:
            return False
        else:
            return True


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_sync(harness):
    sync_tester = SyncTester(harness)
    result, task_id = sync_tester.request_sync_package_preparation()
    assert result, "Failed to start sync package preparation!"

    max_attempt_count = 10
    while True:
        result, sync_package_preparation_state = sync_tester.get_sync_package_preparation_state(task_id)
        assert result, "Failed to receive sync package preparation state!"
        max_attempt_count -= 1
        if sync_package_preparation_state == "finished":
            break
        assert max_attempt_count, "Failed to receive proper sync package preparation state!"
        assert sync_package_preparation_state != "error", "Failed to receive proper sync package preparation state!"
        time.sleep(1)

    result = sync_tester.get_sync_package(task_id)
    assert result, "Failed to get sync package!"

    result = sync_tester.delete_sync_package(task_id)
    assert result, "Failed to delete sync package!"
