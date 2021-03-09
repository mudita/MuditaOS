# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

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


def assert_that_prepare_sms_is_not_sent(harness, phone_number, sms_text):
    # send_message(harness, phone_number, sms_text)
    old_messages = get_message_by_text(harness, sms_text, str(phone_number))
    # prepare sms and send it (add sms to sending queue)
    prepare_sms(harness, sms_text, str(phone_number), SMSType.QUEUED.value)
    # time to send message
    time.sleep(3)
    # check whether message was sent - it shouldn't be sent in this case
    new_messages = get_message_by_text(harness, sms_text, str(phone_number))
    compare_messages(old_messages, new_messages, SMSType.FAILED)


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("usb_unlocked")

def test_cellular_cold_start(harness, phone_number, sms_text):
    # power up can take more then 12s according to QUECTEL documentation
    request_cellular_power_state_change(harness, cellular_states["PowerOn"])
    time.sleep(30)
    ret = request_cellular_state_info(harness)
    assert ret["status"] == status["OK"]
    assert ret["body"]["cellularState"] is "Ready"

    # power down can take more then 29.5s according to QUECTEL documentation
    request_cellular_power_state_change(harness, cellular_states["PowerOff"])
    time.sleep(35)

    ret = request_cellular_state_info(harness)
    assert ret["status"] == status["OK"]
    assert ret["body"]["cellularState"] is "PowerDown"

    assert_that_prepare_sms_is_not_sent(harness, phone_number, sms_text)

    # power up can take more then 12s according to QUECTEL documentation
    request_cellular_power_state_change(harness, cellular_states["PowerOn"])
    time.sleep(30)
    ret = request_cellular_state_info(harness)
    assert ret["status"] == status["OK"]
    assert ret["body"]["cellularState"] is "Ready"

    # send text message
    send_prepared_message(harness, phone_number, sms_text)
    # try to make outgoing call
    call(harness, phone_number, 5)


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("usb_unlocked")

def test_cellular_hot_start(harness, phone_number, sms_text):
    ret = request_cellular_state_info(harness)
    assert ret["status"] == status["OK"]

    request_cellular_power_state_change(harness, cellular_states["HotStart"])

    # wait for changing state
    time.sleep(1)
    ret = request_cellular_state_info(harness)
    assert ret["status"] == status["OK"]
    assert ret["body"]["cellularState"] is not "Ready"

    # power up can take more then 12s according to QUECTEL documentation
    time.sleep(35)
    ret = request_cellular_state_info(harness)
    assert ret["status"] == status["OK"]
    assert ret["body"]["cellularState"] is "Ready"

    # send text message
    send_prepared_message(harness, phone_number, sms_text)
    # try to make outgoing call
    call(harness, phone_number, 5)
