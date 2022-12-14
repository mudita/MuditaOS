# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import os
import pytest
import shutil
from harness import log, utils
from harness.interface.defs import Method, Endpoint
from harness.rt_harness_discovery import get_harness_automatic
from harness.harness import Harness
from harness.api.filesystem import put_file, get_file
from harness.api.update import PhoneReboot, Reboot
from harness.api.device_info import GetDeviceInfo

old_version = None
old_sha = None


def get_version(harness: Harness):
    r = harness.request(Endpoint.DEVICEINFO, Method.GET, {}).response
    version = r.body["version"]
    sha = r.body["gitRevision"]
    return version, sha


@pytest.mark.usefixtures("phone_security_unlocked")
@pytest.mark.rt1051
@pytest.mark.order("first")
def test_perform_update(harness: Harness, update_file_path):
    filename = os.path.basename(update_file_path)
    path = os.path.dirname(update_file_path)

    if filename != "update.tar":
        filename = "update.tar"
        try:
            os.remove(os.path.join(path, filename))
        except OSError:
            pass
        shutil.copyfile(update_file_path, os.path.join(path, filename))
        update_file_path = os.path.join(path, filename)

    global old_version
    global old_sha
    (old_version, old_sha) = get_version(harness)

    put_file(harness, update_file_path, "/sys/user/temp")
    PhoneReboot(Reboot.UPDATE).run(harness)
    assert harness.connection.watch_port_reboot(300)


@pytest.mark.usefixtures("phone_security_unlocked")
@pytest.mark.rt1051
@pytest.mark.order("second")
def test_verify_update():
    global old_version
    global old_sha

    harness = get_harness_automatic(300)

    ret = GetDeviceInfo().run(harness)
    recovery_status_path = ret.response.body["recoveryStatusFilePath"]
    path = os.path.dirname(recovery_status_path)
    filename = os.path.basename(recovery_status_path)

    (new_version, new_sha) = get_version(harness)
    assert new_version != old_version
    assert new_sha != old_sha

    get_file(harness, file_pure=filename, path_local="./", path_pure=path)
    with open(filename) as f:
        line = f.readline()
        print(line)
        assert "successful" in line

    log.info("update done!")
