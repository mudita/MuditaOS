# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import Status
from harness import log
from harness.api.device_info import GetDiagnosticFilesList, DiagnosticsFileList
from harness.request import TransactionError

class DiagnosticsError(RuntimeError):
    pass

@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_device_list_logs(harness):
    ret = GetDiagnosticFilesList(DiagnosticsFileList.LOGS).run(harness)

    if not ret.files:
        raise DiagnosticsError("No log files")

    log.info("Available log files:")
    for file in ret.files:
        log.info(file)


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_device_list_crash_dumps(harness):
    ret = GetDiagnosticFilesList(DiagnosticsFileList.CRASH_DUMPS).run(harness)

    if not ret.files:
        log.warning("No crash dump files")
        return

    log.info("Available crash dump files:")
    for file in ret.files:
        log.info(file)


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_device_list_invalid(harness):
    with pytest.raises(TransactionError, match=r".*" + str(Status.BadRequest.value) + ".*"):
        GetDiagnosticFilesList(DiagnosticsFileList.INVALID).run(harness)

