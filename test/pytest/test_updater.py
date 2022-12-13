# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import os
import pytest
import shutil
import logging
from harness import log, utils
from harness.interface.defs import Method, Endpoint
from harness.request import Transaction, Request, TransactionError
from harness.rt_harness_discovery import get_harness_automatic
from harness.harness import Harness
from harness.api.filesystem import put_file, get_file
from harness.api.update import PhoneReboot, Reboot


def get_version(harness: Harness):
    r = harness.request(Endpoint.DEVICEINFO, Method.GET, {}).response
    version = r.body["version"]
    sha = r.body["gitRevision"]
    return f"version: {version} sha: {sha}"


def disable_some_logs(harness: Harness):
    from harness.interface.defs import PureLogLevel
    for val in ["SysMgrService", "ServiceDesktop_w2", "CellularMux"]:
        ret = harness.request(Endpoint.DEVELOPERMODE, Method.POST, {
            "log": True, "service": val, "level": PureLogLevel.LOGERROR.value})
        log.info(f"{ret.response}")


@pytest.mark.usefixtures("phone_security_unlocked")
@pytest.mark.rt1051
def test_perform_update(harness: Harness, update_file_path):
    filename = os.path.basename(update_file_path)
    path = os.path.dirname(update_file_path)

    if filename is not "update.tar":
        filename = "update.tar"
        try:
            os.remove(os.path.join(path, filename))
        except OSError:
            pass
        shutil.copyfile(update_file_path, os.path.join(path, filename))
        update_file_path = os.path.join(path, filename)

    log.info(get_version(harness))
    put_file(harness, update_file_path, "/user")
    PhoneReboot(Reboot.UPDATE).run(harness)
    assert harness.connection.watch_port_reboot(300)

# @pytest.mark.usefixtures("phone_security_unlocked")
# @pytest.mark.rt1051
# def test_verify_update():
#     harness = get_harness_automatic(300)
#
#     # with utils.Timeout.limit(seconds=10):
#     log.info(get_version(harness))
#     get_file(harness, "updater.log", "./")
#     with open("updater.log") as f:
#         line = f.readline()
#         assert "OK" in line
#
#     log.info("update done!")
