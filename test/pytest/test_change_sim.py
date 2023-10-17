# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status
from test_send_message import get_message_by_text, test_send_message as send_message
import time


@pytest.mark.rt1051
@pytest.mark.two_sim_cards
@pytest.mark.usefixtures("phone_unlocked")
def test_change_sim(harness, phone_number, sms_text):
    simCard = {
        "SIM1": 0,
        "SIM2": 1,
    }

    # change sim to SIM1
    body = {"changeSim": simCard["SIM1"]}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["NoContent"]

    # time to change sim card in pure phone, value is experimentally chosen, no idea how to do it right
    time.sleep(2)

    # retrieve sim info
    body = {"getInfo": "simState"}
    ret = harness.endpoint_request("developerMode", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["selectedSim"] == str(simCard["SIM1"])
    assert ret["body"]["sim"] == str(simCard["SIM1"])

    # send text message using SIM1
    send_message(harness, phone_number, sms_text)

    # change sim to SIM2
    body = {"changeSim": simCard["SIM2"]}
    ret = harness.endpoint_request("developerMode", "put", body)
    assert ret["status"] == status["NoContent"]

    # time to change sim card in pure phone, value is experimentally chosen, no idea how to do it right
    time.sleep(2)

    # retrieve sim info
    body = {"getInfo": "simState"}
    ret = harness.endpoint_request("developerMode", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["selectedSim"] == str(simCard["SIM2"])
    assert ret["body"]["sim"] == str(simCard["SIM2"])

    # send text message using SIM2
    send_message(harness, phone_number, sms_text)
