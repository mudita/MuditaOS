# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import time

from harness import log
from harness.utils import Timeout
from harness.interface.defs import status
from harness.api.factory import FactoryReset
from harness.rt_harness_discovery import get_harness_automatic

def phone_in_onboarding(harness):

    # assert that we are in ApplicationOnBoarding
    log.debug("Assert that we are in ApplicationOnBoarding")
    assert harness.get_application_name() == "ApplicationOnBoarding"

@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_factory_reset(harness):
    ret = FactoryReset().run(harness)

    assert ret.requestDone == True

    try:
        with Timeout.limit(seconds=30):
            log.info("Please, power-cycle the DUT")
            harness.connection.watch_port_reboot(20)
            harness = get_harness_automatic(20)
    except Timeout as e:
        log.error("Factory reset timeout reached")

    time.sleep(5)  # wait for the DUT to be ready

    phone_in_onboarding(harness)
