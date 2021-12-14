# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest

from harness.api.filesystem import get_file

batteryFuelFileName = "batteryFuelGaugeConfig.cfg"
batteryFuelFilePath = "/sys/user/"

@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.rt1051
def test_battery_file(harness):
    """
    Get batteryFuelGaugeConfig.cfg file
    """
    get_file(harness, batteryFuelFileName, "./", batteryFuelFilePath)
