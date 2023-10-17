# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

from harness import log
from harness.interface.defs import SMSType
from harness.interface.defs import status
from test_send_message import get_message_by_text, compare_messages, prepare_sms, test_send_prepared_message as send_prepared_message
from test_call import test_call as call


# default sms type is draft
def request_cellular_power_state_change(harness, new_cellular_state: int):
    body = {"changeCellularStateCmd": new_cellular_state}
    return harness.endpoint_request("developerMode", "put", body)


def request_cellular_state_info(harness):
    body = {"getInfo": "cellularState"}
    return harness.endpoint_request("developerMode", "get", body)


cellular_states = {
    "PowerOff": 1,
    "PowerOn": 2,
    "HotStart": 3
}


def wait_for_cellular_state(harness, state, max_attempts = 10):
    for i in range(max_attempts):
        ret = request_cellular_state_info(harness)
        assert ret["status"] == status["OK"]
        if (ret["body"]["cellularState"] == state):
            return True
        else:
            log.info("cellular is not in " + state + " mode, waiting...")
            time.sleep(2)
    return False


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_cellular_cold_start(harness, phone_number, sms_text):
    ret = request_cellular_state_info(harness)
    assert ret["status"] == status["OK"]
    
    if (ret["body"]["cellularState"] != "Ready"):
        # power up can take more then 12s according to QUECTEL documentation
        request_cellular_power_state_change(harness, cellular_states["PowerOn"])
        time.sleep(30)
        assert wait_for_cellular_state(harness, "Ready") == True, "Cellular initialization failed!"

    # power down can take more then 29.5s according to QUECTEL documentation
    request_cellular_power_state_change(harness, cellular_states["PowerOff"])
    time.sleep(20)
    assert wait_for_cellular_state(harness, "PowerDown") == True, "Cellular shutdown failed!"

    # power up can take more then 12s according to QUECTEL documentation
    request_cellular_power_state_change(harness, cellular_states["PowerOn"])
    time.sleep(30)
    assert wait_for_cellular_state(harness, "Ready") == True, "Cellular initialization failed!"

    # send text message
    send_prepared_message(harness, phone_number, sms_text)
    # try to make outgoing call
    call(harness, phone_number, 5)


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_cellular_hot_start(harness, phone_number, sms_text):
    time.sleep(2)
    request_cellular_power_state_change(harness, cellular_states["HotStart"])

    # wait for changing state
    time.sleep(2)
    ret = request_cellular_state_info(harness)
    assert ret["status"] == status["OK"]
    assert ret["body"]["cellularState"] != "Ready", "Cellular reset failed!"

    # power up can take more then 12s according to QUECTEL documentation
    time.sleep(30)
    assert wait_for_cellular_state(harness, "Ready") == True, "Cellular initialization failed!"

    # send text message
    send_prepared_message(harness, phone_number, sms_text)
    # try to make outgoing call
    call(harness, phone_number, 5)

