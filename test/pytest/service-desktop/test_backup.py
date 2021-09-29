# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import time
from harness import log
from harness.utils import Timeout
from harness.interface.error import Error, TestError
from harness.api.filesystem import get_log_file_with_path
from harness.api.device_info import GetDeviceInfo
from harness.api.backup import BackupInit, BackupGetState

@pytest.mark.service_desktop_test
@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.backup
def test_backup(harness):
    log.debug("Requesting backup");

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
                    get_log_file_with_path(harness, p, "./")
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
