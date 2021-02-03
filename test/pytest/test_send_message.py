# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest

from harness.interface.defs import key_codes, SMSType
from harness.interface.CDCSerial import Keytype


def get_message_by_text(harness, message: str, phone_number: str):
    body = {"messageBody": message, "phoneNumber": phone_number}
    return harness.endpoint_request("messages", "get", body)["body"]


# default sms type is draft
def prepare_sms(harness, message: str, phone_number: str, sms_type: int = 1):
    body = {"smsCommand": "smsAdd", "messageBody": message, "phoneNumber": phone_number, "type": sms_type}
    return harness.endpoint_request("developerMode", "put", body)


def compare_messages(old_messages, new_messages):
    diff_messages = []

    for message in new_messages:
        if message not in old_messages:
            diff_messages.append(message)

    assert len(diff_messages) == 1
    assert SMSType(diff_messages[0]["type"]) == SMSType.OUTBOX


def enter_messages_menu(harness):
    harness.connection.send_key_code(key_codes["enter"])
    harness.open_application("messages")
    if harness.connection.get_window_name() != "ApplicationMessages":
        time.sleep(2)
        assert harness.connection.get_window_name() == "ApplicationMessages"


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_send_message(harness, phone_number, sms_text):
    old_messages = get_message_by_text(harness, sms_text, str(phone_number))

    # enter menu
    enter_messages_menu(harness)

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
        time.sleep(1.2)  # it take horrendous amount of time to go back to thread view
        harness.connection.send_key_code(key_codes["fnRight"])

    time.sleep(3)

    new_messages = get_message_by_text(harness, sms_text, str(phone_number))
    compare_messages(old_messages, new_messages)


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
def test_send_prepared_message(harness, phone_number, sms_text):
    old_messages = get_message_by_text(harness, sms_text, str(phone_number))

    # prepare sms and send it (add sms to sending queue)
    prepare_sms(harness, sms_text, str(phone_number), 0x10)
    # time to send message
    time.sleep(3)
    # check whether message was sent
    new_messages = get_message_by_text(harness, sms_text, str(phone_number))
    compare_messages(old_messages, new_messages)


testdata = [
    "Ala1Ma śżżń 😚",
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA😚"
]


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.parametrize("sms_text", testdata, ids=["special_characters", "long_text"])
def test_send_prepared_draft_message(harness, phone_number, sms_text):
    old_messages = get_message_by_text(harness, sms_text, str(phone_number))

    # prepare sms and send it (add sms to sending queue)
    prepare_sms(harness, sms_text, str(phone_number), SMSType.DRAFT.value)

    # enter menu
    enter_messages_menu(harness)

    # enter thread with draft
    harness.connection.send_key_code(key_codes["enter"])
    # send
    harness.connection.send_key_code(key_codes["enter"])
    # go back to main screen
    harness.connection.send_key_code(key_codes["fnRight"], Keytype.long_press)
    # wait for sms to send
    time.sleep(3)

    # check whether message was sent
    new_messages = get_message_by_text(harness, sms_text, str(phone_number))
    compare_messages(old_messages, new_messages)
