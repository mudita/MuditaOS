# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import time
import pytest
from harness import log
from harness.dom_parser_utils import *
from bt_fixtures import *
from bt_utils import *

@pytest.mark.rt1051
@pytest.mark.usefixtures("bt_state_off")
@pytest.mark.usefixtures("phone_unlocked")
def test_bt_turning_on_off(harness):
    state = bt_get_state(harness)
    assert state["power"] == "off"
    log.info("BT turned off.")

    bt_set_status(harness, "on")

    state = bt_get_state(harness)
    assert state["power"] == "on"
    log.info("BT turned on successfully")

    bt_set_status(harness, "off")

    state = bt_get_state(harness)
    assert state["power"] == "off"
    log.info("BT turned off successfully")

@pytest.mark.rt1051
@pytest.mark.usefixtures("bt_state_on")
@pytest.mark.usefixtures("phone_unlocked")
def test_bt_visibility_on_off(harness):
    state = bt_get_state(harness)
    assert state["power"] == "on"

    visibility = state["visibility"]
    log.info("Device visibility is {}".format(visibility))
    time.sleep(1)
    bt_command(harness, "changeVisibility")

    time.sleep(1)
    state = bt_get_state(harness)
    assert (visibility != state["visibility"])
    visibility = state["visibility"]
    log.info("Device visibility is {}".format(visibility))

    time.sleep(1)
    bt_command(harness, "changeVisibility")

    time.sleep(1)
    state = bt_get_state(harness)
    assert (visibility != state["visibility"])

