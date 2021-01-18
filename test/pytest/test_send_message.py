# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

from harness.interface.defs import key_codes, SMSType


def get_message_by_text(harness, message: str, phone_number: str):
    body = {"messageBody": message, "phoneNumber": phone_number}
    return harness.endpoint_request("messages", "get", body)["body"]


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_send_message(harness, phone_number, sms_text):
    old_messages = get_message_by_text(harness, sms_text.upper(), str(phone_number))

    # enter menu
    harness.connection.send_key_code(key_codes["enter"])
    harness.open_application("messages")
    if harness.connection.get_application_name() != "ApplicationMessages":
        time.sleep(2)
        assert harness.connection.get_application_name() == "ApplicationMessages"

    # create new message
    harness.connection.send_key_code(key_codes["left"])
    # enter phone number
    harness.send_number(str(phone_number))
    # move down to message body
    harness.connection.send_key_code(key_codes["down"])
    # write a message
    harness.send_text(sms_text)
    # send
    harness.connection.send_key_code(key_codes["enter"])
    # go back to main screen
    for _ in range(3):
        time.sleep(1.2) # it take horrendous amount of time to go back to thread view
        harness.connection.send_key_code(key_codes["fnRight"])

    new_messages = get_message_by_text(harness, sms_text.upper(), str(phone_number))
    diff_messages = []

    for message in new_messages:
        if message not in old_messages:
            diff_messages.append(message)

    assert len(diff_messages) == 1
    assert SMSType(diff_messages[0]["type"]) == SMSType.OUTBOX

