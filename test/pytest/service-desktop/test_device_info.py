# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import re
from harness.api.device_info import GetDeviceInfo, GetDiagnosticFilesList, DiagnosticsFileList


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_device_information(harness):
    ret = GetDeviceInfo().run(harness)
#
#     assert int(ret.diag_info["batteryLevel"]) <= 100
#     assert ret.diag_info["batteryState"] in ['1', '2']
#     assert ret.diag_info["selectedSim"] in ['0', '1', '4']
#     assert ret.diag_info["trayState"] == '1'
#     assert 0 <= int(ret.diag_info["signalStrength"]) < 5
#     assert ret.diag_info["accessTechnology"] in ['0', '2', '3', '4', '5', '6', '7', '255']
#     assert 0 <= int(ret.diag_info["networkStatus"]) < 7
#     assert ret.diag_info["networkOperatorName"] is not None
#     assert int(ret.diag_info["deviceSpaceTotal"]) == 14945
#     assert int(ret.diag_info["systemReservedSpace"]) == 2042
#     assert 0 < int(ret.diag_info["usedUserSpace"]) <= 12903
#     assert re.match(r"^(\d|[a-z]){8,40}$", ret.diag_info["gitRevision"])
#     assert ret.diag_info["gitBranch"] is not None
#     assert int(ret.diag_info["currentRTCTime"]) > 1641991996
#     assert re.match(r"^\d\.\d\.\d", ret.diag_info["version"])
#     assert re.match(r"^\d{3,14}$", ret.diag_info["serialNumber"])
#     assert ret.diag_info["caseColour"] in ["gray", "black"]
#     assert ret.diag_info["backupLocation"] == "/sys/user/backup"
#     assert re.match(r"^(\d|[a-zA-Z]){32}$", ret.diag_info["deviceToken"])
#
#
# @pytest.mark.service_desktop_test
# @pytest.mark.usefixtures("phone_unlocked")
# def test_get_list_of_log_files(harness):
#     ret = GetDiagnosticFilesList(DiagnosticsFileList.LOGS).run(harness)
#     assert "/sys/user/logs/MuditaOS.log" in ret.files
#
# @pytest.mark.skip(reason = "Deactivated until we have possibility to generate crash dumps on demand [CP-690]")
# @pytest.mark.service_desktop_test
# @pytest.mark.usefixtures("phone_unlocked")
# def test_get_list_of_crash_dump_files(harness):
#     ret = GetDiagnosticFilesList(DiagnosticsFileList.CRASH_DUMPS).run(harness)
#     assert "/sys/user/crash_dumps/crashdump.hex" in ret.files
