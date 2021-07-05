# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import time
import pytest
from harness import log
from bt_fixtures import *
from bt_utils import *

@pytest.mark.rt1051
@pytest.mark.usefixtures("bt_state_on")
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.skipif("not config.getvalue('--bt_device')", reason='--bt_device was not specified')
def test_bt_pairing_dev(harness, bt_device):
    time.sleep(1)
    bt_command(harness, "scanOn")

    time.sleep(1)
    device = bt_find_device(harness, "scanned", bt_device)
    assert device
    address = device.get('address')

    bt_pair_command(harness, "pair", address, "post")
    time.sleep(1)
    bt_command(harness, "scanOff")

    time.sleep(1)
    device = bt_find_device(harness, "bonded", bt_device)
    assert device
    assert device.get('address') == address

    bt_set_status(harness, "off")
    time.sleep(1)
    bt_set_status(harness, "on")
    time.sleep(1)

    time.sleep(1)
    log.info("Checking if {} is still paired after turning BT off-on...".format(bt_device))
    device = bt_find_device(harness, "bonded", bt_device)
    assert device
    assert device.get('address') == address

    bt_pair_command(harness, "unpair", address, "del")
    assert bt_is_device_forgotten(harness, bt_device)
    log.info("Device {} successfully unpaired.".format(address))


@pytest.mark.rt1051
@pytest.mark.usefixtures("bt_state_on")
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.skipif("not config.getvalue('--bt_device')", reason='--bt_device was not specified')
def test_bt_connection_dev(harness, bt_device):
    time.sleep(1)
    bt_command(harness, "scanOn")

    time.sleep(1)
    device = bt_find_device(harness, "scanned", bt_device)
    assert device
    address = device.get('address')

    bt_pair_command(harness, "pair", address, "post")
    time.sleep(1)
    bt_command(harness, "scanOff")

    time.sleep(1)
    device = bt_find_device(harness, "bonded", bt_device)
    assert device
    assert device.get('address') == address

    bt_pair_command(harness, "connect", address, "post")
    time.sleep(1)
    connected_address = bt_get_connected_address(harness)
    assert connected_address == address

    bt_command(harness, "disconnect", "del")
    assert bt_is_device_disconnected(harness)

    bt_pair_command(harness, "unpair", address, "del")
    assert bt_is_device_forgotten(harness, bt_device)
    log.info("Device {} successfully unpaired.".format(address))
