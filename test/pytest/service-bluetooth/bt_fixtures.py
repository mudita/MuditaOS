# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness import log
from harness.interface.defs import key_codes, status
from harness.dom_parser_utils import *
import time
from bt_utils import *

@pytest.fixture(scope='function')
def bt_main_window(harness):
    current_window_content = get_window_content(harness, 1)
    assert item_contains_recursively(current_window_content, 'WindowName', 'MainWindow' )
    harness.connection.send_key_code(key_codes["enter"])

    log.info("Navigating to ApplicationSettings")
    harness.open_application("settings")
    if harness.connection.get_application_name() != "ApplicationSettings":
        time.sleep(5)
        assert harness.connection.get_application_name() == "ApplicationSettings"

    log.info("Opening Bluetooth")
    harness.connection.send_key_code(key_codes["down"])
    harness.connection.send_key_code(key_codes["enter"])

@pytest.fixture(scope='function')
def bt_reset(harness):
    current_window_content = get_window_content(harness, 1)
    assert item_contains_recursively(current_window_content, 'WindowName', 'Bluetooth' )

    parent_of_list_items = find_parent(current_window_content, 'ListItem')
    if item_has_child_that_contains_recursively( parent_of_list_items, [('TextValue','Bluetooth'), ('TextValue', 'ON')] ) :
        log.info("Bluetooth is ON, turing OFF...")
        harness.connection.send_key_code(key_codes["enter"])

    current_window_content = get_window_content(harness, 1)
    parent_of_list_items = find_parent(current_window_content, 'ListItem')
    assert item_has_child_that_contains_recursively( parent_of_list_items, [('TextValue','Bluetooth'), ('TextValue', 'OFF')] )

    log.info("Turing Bluetooth ON...")
    harness.connection.send_key_code(key_codes["enter"])

@pytest.fixture(scope='function')
def bt_all_devices(harness):
    log.info("Navigating to AllDevices window...")
    time.sleep(1)
    harness.connection.send_key_code(key_codes["down"])
    harness.connection.send_key_code(key_codes["enter"])

    current_window_content = get_window_content(harness, 1)
    assert item_contains_recursively(current_window_content, 'WindowName', 'AllDevices')

@pytest.fixture(scope='function')
def bt_state_on(harness):
    status = bt_get_state(harness)
    if status["power"] == "off":
        bt_set_status(harness, "on")


@pytest.fixture(scope='function')
def bt_state_off(harness):
    status = bt_get_state(harness)
    if status["power"] == "on":
        bt_set_status(harness, "off")
