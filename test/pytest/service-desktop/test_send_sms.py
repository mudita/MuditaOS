# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_send_sms_ok(harness, phone_number, sms_text):
    body = {"category": "message", "number": str(phone_number), "messageBody": sms_text}
    ret = harness.endpoint_request("messages", "post", body)

    assert ret["status"] == status["OK"]

    res_body = ret["body"]
    assert type(res_body["contactID"]) == int
    assert type(res_body["messageBody"]) == str
    assert res_body["messageBody"] == sms_text
    assert type(res_body["messageID"]) == int
    assert type(res_body["messageType"]) == int
    assert type(res_body["createdAt"]) == int
    assert type(res_body["threadID"]) == int


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_send_sms_too_long(harness, phone_number):
    # single msg is 67 chars max
    # we concatenate up to 7 msgs
    sms_text = ''
    for i in range(67*7):
        sms_text = sms_text + chr(48 + i % 10)

    body = {"category": "message", "number": str(phone_number), "messageBody": sms_text}
    ret = harness.endpoint_request("messages", "post", body)
    assert ret["status"] == status["OK"]

    sms_text = sms_text + 'a'
    body = {"category": "message", "number": str(phone_number), "messageBody": sms_text}
    ret = harness.endpoint_request("messages", "post", body)
    assert ret["status"] == status["BadRequest"]


