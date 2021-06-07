# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import time
import pytest
import copy

from harness.interface.defs import key_codes, SMSType, status
from harness.interface.CDCSerial import Keytype

# time for  message to leave the sending queue
extra_time_to_send_message  = 50

def check_for_sent(old_messages,harness, sms_text, phone_number,timeout):
    local_timeout = copy.deepcopy(timeout)
    while local_timeout != 0:

        new_messages = get_message_by_text(harness, sms_text, str(phone_number))
        diff_messages = []

        for message in new_messages:
            if message not in old_messages:
                diff_messages.append(message)

        if len(diff_messages) > 0 and SMSType(diff_messages[0]["messageType"]) == SMSType.OUTBOX:
            return diff_messages
        local_timeout -=1
        time.sleep(1)
    pytest.fail("Message send timeout!")

def erase_all_templates(harness):
    # getting the templates count
    body = {"category": "template", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    count = ret["body"]["count"]

    # getting all templates
    body = {"category": "template", "limit": count, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    assert ret["body"]["totalCount"] == count

    for template in ret["body"]["entries"]:
        body = {"category": "template", "templateID": template["templateID"]}
        del_res = harness.endpoint_request("messages", "del", body)
        assert del_res["status"] == status["NoContent"]


def add_new_template(harness, template_text: str):
    # adding new template
    body = {"category": "template", "templateBody": template_text}
    ret = harness.endpoint_request("messages", "post", body)
    assert ret["status"] == status["NoContent"]


def get_all_contacts(harness):
    body = {"count": True}
    ret = harness.endpoint_request("contacts", "get", body)

    assert ret["status"] == status["OK"]

    contacts = []
    count = ret["body"]["count"]
    if count == 0:
        return contacts

    # try to get more than available
    batch_size = 30
    divider = int((count + 10) / batch_size)
    reminder = (count + 10) % batch_size

    for i in range(divider):
        body = {"count": batch_size, "offset": batch_size * i}
        ret = harness.endpoint_request("contacts", "get", body)
        assert ret["status"] == status["OK"]
        contacts = contacts + ret["body"]["entries"]

    body = {"count": reminder, "offset": (count + 10) - reminder}
    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]
    contacts = contacts + ret["body"]["entries"]
    return contacts


def erase_contacts_by_name(harness, name):
    contacts = get_all_contacts(harness)

    # collecting contacts to remove by name
    ids = []
    for contact in contacts:
        if name in contact["priName"]:
            ids.append(contact["id"])

    # erase all contacts by id
    for identifier in ids:
        # removing added contact
        body = {"id": identifier}
        ret = harness.endpoint_request("contacts", "del", body)
        assert ret["status"] == status["NoContent"]


def erase_contacts_by_phone_number(harness, phone_number):
    contacts = get_all_contacts(harness)

    # collecting contacts to remove by phone number
    ids = []
    for contact in contacts:
        numbers = contact["numbers"]
        for number in numbers:
            if number == phone_number:
                ids.append(contact["id"])
                break

    # erase all contacts by id
    for identifier in ids:
        # removing added contact
        body = {"id": identifier}
        ret = harness.endpoint_request("contacts", "del", body)
        assert ret["status"] == status["NoContent"]


def get_message_by_text(harness, message: str, phone_number: str):
    body = {"category": "message", "messageBody": message, "phoneNumber": phone_number}
    return harness.endpoint_request("messages", "get", body)["body"]


# default sms type is draft
def prepare_sms(harness, message: str, phone_number: str, sms_type: int = 1):
    body = {"smsCommand": "smsAdd", "messageBody": message, "phoneNumber": phone_number, "messageType": sms_type}
    return harness.endpoint_request("developerMode", "put", body)


def prepare_sms_template(harness, message: str, phone_number: str):
    body = {"template": True, "messageBody": message, "phoneNumber": phone_number}
    return harness.endpoint_request("developerMode", "post", body)


def compare_messages(old_messages, new_messages, sms_type: SMSType = SMSType.OUTBOX):
    diff_messages = []

    for message in new_messages:
        if message not in old_messages:
            diff_messages.append(message)

    assert len(diff_messages) == 1
    assert SMSType(diff_messages[0]["messageType"]) == sms_type
    return diff_messages


def enter_messages_menu(harness):
    harness.connection.send_key_code(key_codes["enter"])
    harness.open_application("messages")
    if harness.get_application_name() != "ApplicationMessages":
        time.sleep(2)
        assert harness.get_application_name() == "ApplicationMessages"


def enter_contacts_menu(harness):
    harness.connection.send_key_code(key_codes["enter"])
    harness.open_application("contacts")
    if harness.get_application_name() != "ApplicationPhonebook":
        time.sleep(2)
        assert harness.get_application_name() == "ApplicationPhonebook"


def remove_added_messages(harness, diff_messages):
    for  message in diff_messages:
        body = {"category": "message", "messageID": message["messageID"]}
        del_res = harness.endpoint_request("messages", "del", body)
        assert del_res["status"] == status["NoContent"]


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("phone_in_desktop")
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

    # check if we back to ApplicationDesktop
    assert harness.get_application_name() == "ApplicationDesktop"

    diff_messages = check_for_sent(old_messages,harness,sms_text,phone_number,extra_time_to_send_message)
    # cleaning
    remove_added_messages(harness,diff_messages)



@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("phone_in_desktop")
def test_send_prepared_message(harness, phone_number, sms_text, clean = True):
    old_messages = get_message_by_text(harness, sms_text, str(phone_number))

    # prepare sms and send it (add sms to sending queue)
    prepare_sms(harness, sms_text, str(phone_number), SMSType.QUEUED.value)
    # time to send message
    time.sleep(3)

    diff_messages = check_for_sent(old_messages, harness, sms_text, phone_number, extra_time_to_send_message)

    # cleaning
    if(clean == True):
        remove_added_messages(harness,diff_messages)
    return diff_messages




testdata = [
    "'\"\\àśżżńú😚",
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
    assert harness.get_application_name() == "ApplicationDesktop"

    diff_messages = check_for_sent(old_messages, harness, sms_text, phone_number, extra_time_to_send_message)

    # cleaning
    remove_added_messages(harness,diff_messages)



@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("phone_in_desktop")
def test_send_message_from_template(harness, phone_number, sms_text):
    old_messages = get_message_by_text(harness, sms_text, str(phone_number))
    # erasing all templates
    erase_all_templates(harness)
    # add own (well-known) template
    add_new_template(harness, sms_text)
    # try to send message based on template

    # enter menu
    enter_messages_menu(harness)
    # create new message
    harness.connection.send_key_code(key_codes["left"])
    # enter phone number
    harness.send_number(str(phone_number))
    # move down to message body
    harness.connection.send_key_code(key_codes["down"])
    # move to options
    harness.connection.send_key_code(key_codes["fnLeft"])
    # move to templates
    harness.connection.send_key_code(key_codes["enter"])
    # select template
    harness.connection.send_key_code(key_codes["enter"])
    # send message
    harness.connection.send_key_code(key_codes["enter"])
    # go back to main screen
    harness.connection.send_key_code(key_codes["fnRight"], Keytype.long_press)
    # give some time to back to ApplicationDesktop and send message
    time.sleep(3)
    assert harness.get_application_name() == "ApplicationDesktop"

    diff_messages = check_for_sent(old_messages, harness, sms_text, phone_number, extra_time_to_send_message)

    # cleaning
    remove_added_messages(harness,diff_messages)


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("phone_in_desktop")
def test_forward_message(harness, phone_number, sms_text):
    # send first message in order to forward it
    diff_messages_org = test_send_prepared_message(harness, phone_number, sms_text, False)

    old_messages = get_message_by_text(harness, sms_text, str(phone_number))

    # enter menu
    enter_messages_menu(harness)
    # enter thread with sent message
    harness.connection.send_key_code(key_codes["enter"])
    # select message
    harness.connection.send_key_code(key_codes["up"])
    # move to options
    harness.connection.send_key_code(key_codes["fnLeft"])
    # move down to forward message option (2 times down)
    for _ in range(2):
        harness.connection.send_key_code(key_codes["down"])

    # enter thread with sent message
    harness.connection.send_key_code(key_codes["enter"])
    # go to number field
    harness.connection.send_key_code(key_codes["up"])
    # enter phone number
    harness.send_number(str(phone_number))
    # go to sms body
    harness.connection.send_key_code(key_codes["down"])
    # send message
    harness.connection.send_key_code(key_codes["enter"])
    # go back to main screen
    harness.connection.send_key_code(key_codes["fnRight"], Keytype.long_press)
    # give some time to back to ApplicationDesktop and send message
    time.sleep(3)
    assert harness.get_application_name() == "ApplicationDesktop"

    diff_messages = check_for_sent(old_messages, harness, sms_text, phone_number, extra_time_to_send_message)

    # cleaning
    remove_added_messages(harness,diff_messages)
    remove_added_messages(harness,diff_messages_org)


@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("phone_in_desktop")
def test_resend_message(harness, phone_number, sms_text):
    # send first message in order to resend it
    prepare_sms(harness, sms_text, str(phone_number), SMSType.FAILED.value)

    old_messages = get_message_by_text(harness, sms_text, str(phone_number))

    # enter menu
    enter_messages_menu(harness)
    # enter thread with sent message
    harness.connection.send_key_code(key_codes["enter"])
    # select message
    harness.connection.send_key_code(key_codes["up"])
    # move to options
    harness.connection.send_key_code(key_codes["fnLeft"])
    # resending message
    harness.connection.send_key_code(key_codes["enter"])
    # go back to main screen
    harness.connection.send_key_code(key_codes["fnRight"], Keytype.long_press)
    # give some time to back to ApplicationDesktop and send message
    time.sleep(3)

    # check if we back to ApplicationDesktop
    assert harness.get_application_name() == "ApplicationDesktop"

    diff_messages = check_for_sent(old_messages, harness, sms_text, phone_number, extra_time_to_send_message)

    # cleaning
    remove_added_messages(harness,diff_messages)



@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("phone_in_desktop")
def test_send_message_from_phonebook(harness, phone_number, sms_text):
    # erase Test contacts
    erase_contacts_by_name(harness, "Test")
    # erase Test contacts
    erase_contacts_by_phone_number(harness, "Test")

    old_messages = get_message_by_text(harness, sms_text, str(phone_number))
    # adding new test contact
    body = {"address": "6 Czeczota St.\n02600 Warsaw",
            "altName": "Testowy",
            "blocked": False,
            "favourite": True,
            "numbers": [str(phone_number)],
            "priName": "Test"}
    ret = harness.endpoint_request("contacts", "put", body)
    assert ret["status"] == status["NoContent"]
    added_contact_id = ret["body"]["id"]

    # enter contacts (phonebook) menu
    enter_contacts_menu(harness)

    # search test contact
    # select message
    harness.connection.send_key_code(key_codes["right"])
    # write search text
    harness.send_text("Test")
    # search for added contact
    harness.connection.send_key_code(key_codes["enter"])
    # enter contact
    harness.connection.send_key_code(key_codes["enter"])
    # go to messages in contact
    harness.connection.send_key_code(key_codes["right"])
    # enter new message window
    harness.connection.send_key_code(key_codes["enter"])

    assert harness.get_application_name() == "ApplicationMessages"

    # go to text in new message windows
    harness.connection.send_key_code(key_codes["down"])

    # write a message
    harness.send_text(sms_text)
    # send
    harness.connection.send_key_code(key_codes["enter"])
    # go back to main screen
    harness.connection.send_key_code(key_codes["fnRight"], Keytype.long_press)
    # give some time to back to ApplicationDesktop and send message
    time.sleep(3)
    # check if we back to ApplicationDesktop
    assert harness.get_application_name() == "ApplicationDesktop"

    diff_messages = check_for_sent(old_messages, harness, sms_text, phone_number, extra_time_to_send_message)

    # cleaning
    remove_added_messages(harness,diff_messages)

    # removing added contact
    body = {"id": added_contact_id}
    ret = harness.endpoint_request("contacts", "del", body)
    assert ret["status"] == status["NoContent"]

@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.usefixtures("phone_in_desktop")
def test_send_message_using_phonebook(harness, phone_number, sms_text):
    # erase Test contacts
    erase_contacts_by_name(harness, "Test")
    # erase Test contacts
    erase_contacts_by_phone_number(harness, str(phone_number))

    old_messages = get_message_by_text(harness, sms_text, str(phone_number))
    # adding new test contact
    body = {"address": "6 Czeczota St.\n02600 Warsaw",
            "altName": "Testowy",
            "blocked": False,
            "favourite": True,
            "numbers": [str(phone_number)],
            "priName": "Test"}
    ret = harness.endpoint_request("contacts", "put", body)
    assert ret["status"] == status["NoContent"]
    added_contact_id = ret["body"]["id"]

    # enter messages menu
    enter_messages_menu(harness)
    # create new message
    harness.connection.send_key_code(key_codes["left"])
    # select contact
    harness.connection.send_key_code(key_codes["enter"])
    # search test contact
    harness.connection.send_key_code(key_codes["right"])
    # write search text
    harness.send_text("Test")
    # search for added contact
    harness.connection.send_key_code(key_codes["enter"])
    # choose contact
    harness.connection.send_key_code(key_codes["enter"])
    # reset contact
    harness.connection.send_key_code(key_codes["#"])
    # select contact
    harness.connection.send_key_code(key_codes["enter"])
    # search test contact
    harness.connection.send_key_code(key_codes["right"])
    # write search text
    harness.send_text("Test")

    # search for added contact
    harness.connection.send_key_code(key_codes["enter"])
    # select contact
    harness.connection.send_key_code(key_codes["enter"])

    # go to text in new message windows
    harness.connection.send_key_code(key_codes["down"])

    # write a message
    harness.send_text(sms_text)
    # send
    harness.connection.send_key_code(key_codes["enter"])
    # go back to main screen
    harness.connection.send_key_code(key_codes["fnRight"], Keytype.long_press)

    time.sleep(3)
    # check if we back to ApplicationDesktop
    assert harness.get_application_name() == "ApplicationDesktop"

    diff_messages = check_for_sent(old_messages, harness, sms_text, phone_number, extra_time_to_send_message)

    # cleaning
    remove_added_messages(harness,diff_messages)

    # removing added contact
    body = {"id": added_contact_id}
    ret = harness.endpoint_request("contacts", "del", body)
    assert ret["status"] == status["NoContent"]


