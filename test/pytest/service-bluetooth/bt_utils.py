# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.interface.defs import status
from harness import log
import time

def bt_get_state(harness):
    time.sleep(1)
    body = {"state": True}
    ret = harness.endpoint_request("bluetooth", "get", body)
    assert ret["status"] == status["OK"]
    return ret["body"]["state"]

def bt_set_status(harness, power, visibility = "off"):
    log.info("Turning Bluetooth {} with visibility {}...".format(power, visibility))
    body = {"state": {"power" : power , "visibility" : visibility }}
    ret = harness.endpoint_request("bluetooth", "put", body)
    assert ret["status"] == status["OK"]

def bt_command(harness, command,  http_method = "put"):
    log.info("Sending command: {} with http_method={}".format(command, http_method))
    body = {"command": command}
    ret = harness.endpoint_request("bluetooth", http_method, body)
    assert ret["status"] == status["OK"]

def bt_pair_command(harness, pair_command, address, http_method):
    log.info("Requesting {}ing with address={}...".format(pair_command, address))
    body = {pair_command: address}
    ret = harness.endpoint_request("bluetooth", http_method, body)
    assert ret["status"] == status["OK"]

def bt_get_device_by_name(devices, name) -> dict :
    for device in devices:
        if device["name"] == name :
            return device
    return {}

def bt_find_device(harness, device_origin, device_name, max_attempts = 7):
    log.info("Getting {} devices".format(device_origin))
    body = {"devices": device_origin}

    for i in range(max_attempts):
        ret = harness.endpoint_request("bluetooth", "get", body)
        device = bt_get_device_by_name(ret["body"]["devices"], device_name)
        if device:
            log.info("Found {}, address={}".format(device_name, device.get('address')))
            return device
        if i != max_attempts - 1:
            log.info("Device {} not found, retrying...".format(device_name))
            time.sleep(2)
    return {}

def bt_is_device_forgotten(harness, device_name, max_attempts = 7):
    log.info("Checking if pair forgetting succeeded...")
    body = {"devices": "bonded"}

    for i in range(max_attempts):
        ret = harness.endpoint_request("bluetooth", "get", body)
        device = bt_get_device_by_name(ret["body"]["devices"], device_name)
        if not device:
            return True
        if i != max_attempts - 1:
            log.info("Device {} still paired, retrying...".format(device_name))
            time.sleep(2)
    return False

def bt_get_connected_address(harness, max_attempts = 10):
    log.info("Getting address of connected device")
    body = {"devices": "bonded"}

    for i in range(max_attempts):
        ret = harness.endpoint_request("bluetooth", "get", body)
        address = ret["body"]["address"]
        if len(address) > 0:
            log.info("Device connected={}".format(address))
            return address
        if i != max_attempts - 1:
            log.info("No device connected, retrying...")
            time.sleep(2)
    return ""

def bt_is_device_disconnected(harness, max_attempts = 7):
    log.info("Checking if disconnecting succeeded...")
    body = {"devices": "bonded"}

    for i in range(max_attempts):
        ret = harness.endpoint_request("bluetooth", "get", body)
        address = ret["body"]["address"]
        if len(address) == 0:
            return True
        if i != max_attempts - 1:
            log.info("Device {} connected, retrying...".format(address))
            time.sleep(2)
    return False
