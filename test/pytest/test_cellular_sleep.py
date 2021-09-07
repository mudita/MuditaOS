# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest
from harness import log
from harness.interface.defs import status
from harness.interface.defs import key_codes
from test_call import  test_call as call
from test_send_message import test_send_prepared_message as send_prepared_message

def request_cellular_sleep_mode_info(harness):
    body = {"getInfo": "cellularSleepMode"}
    return harness.endpoint_request("developerMode", "get", body)
    

def wait_for_cellular_sleep(harness, max_attempts):
    for i in range(max_attempts):
        ret = request_cellular_sleep_mode_info(harness)
        assert ret["status"] == status["OK"]
        if  ret["body"]["cellularSleepMode"] == True:
            return True
        else:
            log.info("cellular is not in the sleep mode, waiting...")
            time.sleep(2)
    return False


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_call_during_cellular_sleep_mode(harness, phone_number, call_duration, max_attempts = 10):
    cellular_sleep_mode = wait_for_cellular_sleep(harness, max_attempts)
    if (cellular_sleep_mode == False):
        log.warning("cellular is not in the sleep mode!")

    harness.unlock_phone()
    # try to make outgoing call
    call(harness, phone_number, call_duration)
    
    # cellular should be awake 
    ret = request_cellular_sleep_mode_info(harness)
    assert ret["status"] == status["OK"]
    assert ret["body"]["cellularSleepMode"] == False

@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_sms_during_cellular_sleep_mode(harness, phone_number, sms_text, max_attempts = 10):
    cellular_sleep_mode = wait_for_cellular_sleep(harness, max_attempts)
    if (cellular_sleep_mode == False):
        log.warning("cellular is not in the sleep mode!")
    
    # send text message
    send_prepared_message(harness, phone_number, sms_text)
    
    # cellular should be awake 
    ret = request_cellular_sleep_mode_info(harness)
    assert ret["status"] == status["OK"]
    assert ret["body"]["cellularSleepMode"] == False
