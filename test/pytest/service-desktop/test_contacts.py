# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
def test_contacts(harness):
    # getting the contacts count
    body = {"count": True}
    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]

    count = ret["body"]["count"]
    if count == 0:
        pytest.skip("No contacts entries, skipping")

    # getting all contacts
    body = {"count": count}
    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]

    contacts = ret["body"]
    contacts_length = len(contacts)
    assert contacts_length
    assert contacts_length == count

    # adding new contact
    body = {"address": "6 Czeczota St.\n02600 Warsaw",
            "altName": "Testowy",
            "blocked": True,
            "favourite": True,
            "numbers": ["547623521"],
            "priName": "Test"}
    ret = harness.endpoint_request("contacts", "put", body)
    assert ret["status"] == status["OK"]
    contact_id_to_update = ret["body"]["id"]
    assert contact_id_to_update

    # adding new contact without number - should fail with 406
    body = {"address": "6 Czeczota St.\n02600 Warsaw",
            "altName": "Testowy",
            "blocked": True,
            "favourite": True,
            "numbers": [],
            "priName": "Test"}
    ret = harness.endpoint_request("contacts", "put", body)
    assert ret["status"] == status["NotAcceptable"]

    # checking count after adding
    body = {"count": True}
    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["count"] == count + 1

    # updating existing contact
    body = {"address": "6 Czeczota St.\n02600 Warsaw",
            "altName": "Testowy2",
            "blocked": True,
            "favourite": True,
            "numbers": ["547623521"],
            "priName": "Test2",
            "id": contact_id_to_update}
    ret = harness.endpoint_request("contacts", "post", body)
    assert ret["status"] == status["OK"]

    # gathering updated contact
    body = {"id": contact_id_to_update}
    ret = harness.endpoint_request("contacts", "get", body)
    contact = {"address": "6 Czeczota St.\n02600 Warsaw",
               "altName": "Testowy2",
               "blocked": True,
               "favourite": True,
               "numbers": ["547623521"],
               "priName": "Test2",
               "id": contact_id_to_update}
    assert ret["body"] == contact

    # removing added contact
    body = {"id": contact_id_to_update}
    ret = harness.endpoint_request("contacts", "del", body)
    assert ret["status"] == status["OK"]

    # verifying count
    body = {"count": True}
    ret = harness.endpoint_request("contacts", "get", body)
    assert ret["status"] == status["OK"]

    assert ret["body"]["count"] == count
