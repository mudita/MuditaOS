# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import pytest
from harness.request import TransactionError
from harness.api.messages import AddMessage


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_send_sms_ok(harness, phone_number, sms_text):

    resp = AddMessage(str(phone_number), sms_text).run(harness)

    message = resp.message

    assert type(message["messageBody"]) == str
    assert message["messageBody"] == sms_text
    assert type(message["messageID"]) == int
    assert type(message["messageType"]) == int
    assert type(message["createdAt"]) == int
    assert type(message["threadID"]) == int


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_send_sms_too_long(harness, phone_number):
    # single msg is 67 chars max
    # we concatenate up to 7 msgs
    sms_text = ''
    for i in range(67*7):
        sms_text = sms_text + chr(48 + i % 10)

    ret = AddMessage(str(phone_number), sms_text).run(harness)

    sms_text = sms_text + "a"
    with pytest.raises(TransactionError, match=r".*" + str(Status.BadRequest.value) + ".*"):
        ret = AddMessage(str(phone_number), sms_text).run(harness)
