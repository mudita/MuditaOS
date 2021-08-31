# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_contacts(harness):
    # getting the contacts count
    body = {"count": True}
    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]

    count = ret["body"]["count"]
    if count == 0:
        pytest.skip("No contacts entries, skipping")

    # getting all contacts
    batch_size = 10
    divider = int(count / batch_size)
    reminder = count % batch_size
    contacts = []
    for i in range(divider):
        body = {"limit": batch_size, "offset": batch_size*i}
        ret = harness.endpoint_request("contacts", "get", body)
        assert ret["status"] == status["OK"]
        contacts = contacts + ret["body"]["entries"]

    if reminder != 0:
        body = {"limit": reminder, "offset": count-reminder}
        ret = harness.endpoint_request("contacts", "get", body)
        assert ret["status"] == status["OK"]
        contacts = contacts + ret["body"]["entries"]

    contacts_length = len(contacts)
    assert contacts_length
    assert contacts_length == count

    # try to get more than available
    batch_size = 10
    divider = int((count+10) / batch_size)
    reminder = (count+10) % batch_size
    contacts = []
    for i in range(divider):
        body = {"limit": batch_size, "offset": batch_size * i}
        ret = harness.endpoint_request("contacts", "get", body)
        assert ret["status"] == status["OK"]
        contacts = contacts + ret["body"]["entries"]

    if reminder !=0:
        body = {"limit": reminder, "offset": (count+10)-reminder}
    else:
        body = {"limit": 10, "offset": count}
    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]
    contacts = contacts + ret["body"]["entries"]

    contacts_length = len(contacts)
    assert contacts_length
    assert contacts_length == count

    # adding new contact
    body = {"address": "6 Czeczota St.\n02600 Warsaw",
            "altName": "Testowy",
            "email": "testowy.2@example.com",
            "blocked": True,
            "favourite": True,
            "ice": False,
            "numbers": ["547623521"],
            "priName": "Test"}
    ret = harness.endpoint_request("contacts", "post", body)
    assert ret["status"] == status["OK"]
    contact_id_to_update = ret["body"]["id"]
    assert contact_id_to_update

    # try to add duplicate
    body = {"address": "6 Czeczota St.\n02600 Warsaw",
            "altName": "Testowy",
            "blocked": True,
            "favourite": True,
            "numbers": ["547623521"],
            "priName": "Test"}
    ret = harness.endpoint_request("contacts", "post", body)
    assert ret["status"] == status["Conflict"]
    contact_id_of_detected_duplicate = ret["body"]["id"]
    assert contact_id_of_detected_duplicate == contact_id_to_update

    # adding new contact without number - should fail with 406
    body = {"address": "6 Czeczota St.\n02600 Warsaw",
            "altName": "Testowy",
            "blocked": True,
            "favourite": True,
            "numbers": [],
            "priName": "Test"}
    ret = harness.endpoint_request("contacts", "post", body)
    assert ret["status"] == status["NotAcceptable"]

    # checking count after adding
    body = {"count": True}
    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["count"] == count + 1

    # updating existing contact
    body = {"address": "6 Czeczota St.\n02600 Warsaw",
            "altName": "Testowy2",
            "email": "testowy.2@example.com",
            "blocked": False,
            "favourite": True,
            "ice": True,
            "numbers": ["547623521"],
            "speedDial": "7",
            "priName": "Test2",
            "note": "this is a really cool guy",
            "id": contact_id_to_update}
    ret = harness.endpoint_request("contacts", "put", body)
    assert ret["status"] == status["NoContent"]

    # gathering updated contact
    body = {"id": contact_id_to_update}
    ret = harness.endpoint_request("contacts", "get", body)
    contact = {"address": "6 Czeczota St.\n02600 Warsaw",
               "altName": "Testowy2",
               "email": "testowy.2@example.com",
               "blocked": False,
               "favourite": True,
               "ice": True,
               "numbers": ["547623521"],
               "speedDial": "7",
               "priName": "Test2",
               "note": "this is a really cool guy",
               "id": contact_id_to_update}
    assert ret["body"] == contact

    # removing added contact
    body = {"id": contact_id_to_update}
    ret = harness.endpoint_request("contacts", "del", body)
    assert ret["status"] == status["NoContent"]

    # verifying count
    body = {"count": True}
    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]

    assert ret["body"]["count"] == count
