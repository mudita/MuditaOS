# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

from harness.interface.defs import key_codes


def get_message_by_text(harness, message: str):
    body = {"messageBody": message}
    return harness.endpoint_request("messages", "get", body)["body"]


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_send_message(harness, phone_number, sms_text):
    messages = get_message_by_text(harness, sms_text.upper())

    # enter menu
    harness.connection.send_key(key_codes["enter"])
    harness.open_application("messages")
    if harness.connection.get_window_name() != "ApplicationMessages":
        time.sleep(2)
        assert harness.connection.get_window_name() == "ApplicationMessages"

    # create new message
    harness.connection.send_key(key_codes["left"])
    # enter phone number
    harness.send_number(str(phone_number))
    # move down to message body
    harness.connection.send_key(key_codes["down"])
    # write a message
    harness.send_text(sms_text)
    # send
    harness.connection.send_key(key_codes["enter"])

    # go back to main screen
    for i in range(3):
        harness.connection.send_key(key_codes["fnRight"])

    time.sleep(2)
    new_messages = get_message_by_text(harness, sms_text.upper())

    diff = [i for i in messages + new_messages if i not in messages or i not in new_messages]
    assert len(diff) == 1
    assert diff[0]["type"] == 0x08
