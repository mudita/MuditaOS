# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import os
import pytest
import logging
from harness import log
from harness.interface.defs import Method, Endpoint
from harness.request import Transaction, Request, TransactionError
from harness.rt_harness_discovery import get_harness_automatic
from harness.harness import Harness
from harness.api.filesystem import put_file, get_file
from harness.api.developermode import PhoneModeLock
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


# @pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_update(harness: Harness):
    filename = "update.tar"

    log.info(get_version(harness))
    # PhoneModeLock(False).run(harness)
    put_file(harness, filename, "/sys/user")
    PhoneReboot(Reboot.UPDATE).run(harness)
    assert harness.connection.watch_port_reboot(300)

    harness = get_harness_automatic(300)
    import time
    time.sleep(15)
    # harness.unlock_phone()
    # PhoneModeLock(False).run(harness)

    log.info(get_version(harness))
    get_file(harness, "updater.log", "./")
    with open("updater.log") as f:
        line = f.readline()
        assert "OK" in line
    # PhoneModeLock(True).run(harness)

    log.info("update done!")
