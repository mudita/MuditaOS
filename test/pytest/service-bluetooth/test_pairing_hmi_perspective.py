# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import time
import pytest
from harness import log
from harness.dom_parser_utils import *
from harness.interface.defs import key_codes
from bt_fixtures import *

@pytest.mark.rt1051
@pytest.mark.usefixtures("bt_all_devices")
@pytest.mark.usefixtures("bt_reset")
@pytest.mark.usefixtures("bt_main_window")
@pytest.mark.usefixtures("phone_in_desktop")
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.skipif("not config.getvalue('--bt_device')", reason='--bt_device was not specified')
def test_bt_pairing_hmi(harness, bt_device):
    if not bt_device:
        return
    bt_device_name = bt_device

    current_window_content = get_window_content(harness, 1)
    is_device_in_history = item_contains_recursively(current_window_content, 'TextValue', bt_device_name )

    if not is_device_in_history :
        log.info("Device {} not in all devices history, scanning...".format(bt_device_name))
        harness.connection.send_key_code(key_codes["left"])
        max_try_count = 5
        for _ in range(max_try_count, 0, -1) :
            time.sleep(2)
            current_window_content = get_window_content(harness, 1)
            is_device_in_history = item_contains_recursively(current_window_content, 'TextValue', bt_device_name )

            if is_device_in_history:
                break
            log.info("Device {} not found, retrying...".format(bt_device_name))

        assert max_try_count

    current_window_content = get_window_content(harness, 1)
    parent_of_list_items = find_parent(current_window_content, 'ListItem')
    steps_to_navigate_down = get_child_number_that_contains_recursively(parent_of_list_items, [('TextValue', bt_device_name)])
    assert steps_to_navigate_down > -1
    log.info("Navigating to the {} device, {} down".format(bt_device_name, steps_to_navigate_down ) )
    for _ in range(steps_to_navigate_down) :
        harness.connection.send_key_code(key_codes["down"])

    log.info("Checking if device {} is focused...".format(bt_device_name))
    current_window_content = get_window_content(harness, 1)
    parent_of_list_items = find_parent(current_window_content, 'ListItem')
    assert item_has_child_that_contains_recursively( parent_of_list_items, [('TextValue', bt_device_name), ('Focus', True)] )

