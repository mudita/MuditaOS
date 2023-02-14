# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status
from harness.request import TransactionError
from harness.api.contacts import GetContactsCount, GetContactsWithOffsetAndLimit, GetContactById, AddContact, \
    UpdateContact, DeleteContactById


class ContactsTester:
    def __init__(self, harness):
        self.harness = harness

    def get_contacts_count(self):
        try:
            count = GetContactsCount().run(self.harness).count
        except TransactionError:
            return False
        else:
            return True, count

    def get_contacts_with_offset_and_limit(self, offset, limit):
        try:
            result = GetContactsWithOffsetAndLimit(offset, limit).run(self.harness)
        except TransactionError:
            return False
        else:
            return True, result.entries, result.totalCount

    def get_contact_by_id(self, contact_record_id):
        try:
            result = GetContactById(contact_record_id).run(self.harness)
        except TransactionError:
            return False
        else:
            return True, result.entry

    def add_contact(self, contact_record):
        try:
            contact_id = AddContact(contact_record).run(self.harness).id
        except TransactionError:
            return False, -1
        else:
            return True, contact_id

    def detect_adding_duplicated_contact(self, contact_record):
        try:
            AddContact(contact_record).run(self.harness)
        except TransactionError as error:
            if error.status == status["Conflict"]:
                return True
        else:
            return False

    def update_contact(self, contact_record):
        try:
            UpdateContact(contact_record).run(self.harness)
        except TransactionError:
            return False
        else:
            return True

    def delete_contact_by_id(self, contact_record_id):
        try:
            DeleteContactById(contact_record_id).run(self.harness)
        except TransactionError:
            return False
        else:
            return True


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_add_single_contact(harness):
    # Check initial state
    contacts_tester = ContactsTester(harness)
    result, initial_number_of_contact_records = contacts_tester.get_contacts_count()
    assert result, "Failed to get contacts count!"

    # Add contact
    contact_record = {
        "address": "6 Czeczota St.\n02600 Warsaw",
        "altName": "Smith",
        "email": "john.smith@mudita.com",
        "blocked": False,
        "favourite": False,
        "ice": False,
        "numbers": [
            "123456789"
        ],
        "speedDial": "1",
        "priName": "John",
        "note": "Some note"
    }

    result, contact_id = contacts_tester.add_contact(contact_record)
    assert result, "Failed to add contact!"
    result, retrieved_contact_record = contacts_tester.get_contact_by_id(contact_id)
    assert result, "Failed to get contact by ID!"
    del retrieved_contact_record["id"]
    contact_record["numbersIDs"] = retrieved_contact_record["numbersIDs"]
    assert contact_record == retrieved_contact_record, "Received contact record is different than expected!"

    # Clean up
    assert contacts_tester.delete_contact_by_id(contact_id), "Failed to delete a contact!"
    result, received_contact_records_count = contacts_tester.get_contacts_count()
    assert result, "Failed to get contacts count!"
    assert initial_number_of_contact_records == received_contact_records_count, "Incorrect contact records number!"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_add_duplicated_contact(harness):
    # Check initial state
    contacts_tester = ContactsTester(harness)
    result, initial_number_of_contact_records = contacts_tester.get_contacts_count()
    assert result, "Failed to get contacts count!"

    # Add first contact
    contact_record = {
        "address": "6 Czeczota St.\n02600 Warsaw",
        "altName": "Smith",
        "email": "john.smith@mudita.com",
        "blocked": False,
        "favourite": False,
        "ice": False,
        "numbers": [
            "123456789"
        ],
        "speedDial": "1",
        "priName": "John",
        "note": "Some note"
    }

    result, contact_id = contacts_tester.add_contact(contact_record)
    assert result, "Failed to add contact!"

    # Attempt to add duplicate contact
    assert contacts_tester.detect_adding_duplicated_contact(contact_record), "Failed to detect duplicate!"

    # Clean up
    assert contacts_tester.delete_contact_by_id(contact_id), "Failed to delete a contact!"
    result, received_contact_records_count = contacts_tester.get_contacts_count()
    assert result, "Failed to get contacts count!"
    assert initial_number_of_contact_records == received_contact_records_count, "Incorrect contact records number!"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_edit_contact(harness):
    # Check initial state
    contacts_tester = ContactsTester(harness)
    result, initial_number_of_contact_records = contacts_tester.get_contacts_count()
    assert result, "Failed to get contacts count!"

    # Add contact
    contact_record = {
        "address": "6 Czeczota St.\n02600 Warsaw",
        "altName": "Smith",
        "email": "john.smith@mudita.com",
        "blocked": False,
        "favourite": False,
        "ice": False,
        "numbers": [
            "123456789"
        ],
        "speedDial": "1",
        "priName": "John",
        "note": "Some note"
    }

    result, contact_id = contacts_tester.add_contact(contact_record)
    assert result, "Failed to add contact!"

    # Edit contact
    edited_contact_record = {
        "id": contact_id,
        "address": "6 Czeczota St.\n02600 Warsaw",
        "altName": "Davies",
        "email": "john.davies@mudita.com",
        "blocked": False,
        "favourite": False,
        "ice": False,
        "numbers": [
            "123456799"
        ],
        "speedDial": "1",
        "priName": "John",
        "note": "Some note"
    }
    assert contacts_tester.update_contact(edited_contact_record), "Failed to update contact!"
    result, retrieved_contact_record = contacts_tester.get_contact_by_id(contact_id)
    edited_contact_record["numbersIDs"] = retrieved_contact_record["numbersIDs"]
    assert result, "Failed to get contact by ID!"
    assert retrieved_contact_record == edited_contact_record

    # Clean up
    assert contacts_tester.delete_contact_by_id(contact_id), "Failed to delete a contact!"
    result, received_contact_records_count = contacts_tester.get_contacts_count()
    assert result, "Failed to get contacts count!"
    assert initial_number_of_contact_records == received_contact_records_count, "Incorrect contact records number!"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_add_and_get_multiple_contacts(harness):
    # Check initial state
    contacts_tester = ContactsTester(harness)
    result, received_contact_records_count = contacts_tester.get_contacts_count()
    assert result, "Failed to get contacts count!"
    initial_number_of_contact_records = received_contact_records_count

    # Add contacts
    first_contact_record = {
        "address": "6 Czeczota St.\n02600 Warsaw",
        "altName": "Smith",
        "email": "john.smith@mudita.com",
        "blocked": False,
        "favourite": False,
        "ice": False,
        "numbers": [
            "123456789"
        ],
        "speedDial": "1",
        "priName": "John",
        "note": "Some note"
    }

    result, first_contact_id = contacts_tester.add_contact(first_contact_record)
    assert result, "Failed to add contact!"
    result, retrieved_contact_record = contacts_tester.get_contact_by_id(first_contact_id)
    assert result, "Failed to get contact by ID!"
    del retrieved_contact_record["id"]
    first_contact_record["numbersIDs"] = retrieved_contact_record["numbersIDs"]
    assert first_contact_record == retrieved_contact_record, "Received contact record is different than expected!"
    current_number_of_contact_records = initial_number_of_contact_records + 1

    second_contact_record = {
        "address": "6 Czeczota St.\n02600 Warsaw",
        "altName": "Johns",
        "email": "john.johns@mudita.com",
        "blocked": False,
        "favourite": False,
        "ice": False,
        "numbers": [
            "223456789"
        ],
        "speedDial": "2",
        "priName": "John",
        "note": "Some note"
    }

    result, second_contact_id = contacts_tester.add_contact(second_contact_record)
    assert result, "Failed to add contact!"
    result, retrieved_contact_record = contacts_tester.get_contact_by_id(second_contact_id)
    assert result, "Failed to get contact by ID!"
    del retrieved_contact_record["id"]
    second_contact_record["numbersIDs"] = retrieved_contact_record["numbersIDs"]
    assert second_contact_record == retrieved_contact_record, "Received contact record is different than expected!"
    current_number_of_contact_records += 1

    third_contact_record = {
        "address": "6 Czeczota St.\n02600 Warsaw",
        "altName": "Williams",
        "email": "george.williams@mudita.com",
        "blocked": False,
        "favourite": False,
        "ice": False,
        "numbers": [
            "323456789"
        ],
        "speedDial": "3",
        "priName": "George",
        "note": "Some note"
    }

    result, third_contact_id = contacts_tester.add_contact(third_contact_record)
    assert result, "Failed to add contact!"
    result, retrieved_contact_record = contacts_tester.get_contact_by_id(third_contact_id)
    assert result, "Failed to get contact by ID!"
    del retrieved_contact_record["id"]
    third_contact_record["numbersIDs"] = retrieved_contact_record["numbersIDs"]
    assert third_contact_record == retrieved_contact_record, "Received contact record is different than expected!"
    current_number_of_contact_records += 1

    fourth_contact_record = {
        "address": "6 Czeczota St.\n02600 Warsaw",
        "altName": "Brown",
        "email": "olivia.brown@mudita.com",
        "blocked": False,
        "favourite": False,
        "ice": False,
        "numbers": [
            "423456789"
        ],
        "speedDial": "4",
        "priName": "Olivia",
        "note": "Some note"
    }

    result, fourth_contact_id = contacts_tester.add_contact(fourth_contact_record)
    assert result, "Failed to add contact!"
    result, retrieved_contact_record = contacts_tester.get_contact_by_id(fourth_contact_id)
    assert result, "Failed to get contact by ID!"
    del retrieved_contact_record["id"]
    fourth_contact_record["numbersIDs"] = retrieved_contact_record["numbersIDs"]
    assert fourth_contact_record == retrieved_contact_record, "Received contact record is different than expected!"
    current_number_of_contact_records += 1

    # Check contacts counter
    result, received_contact_records_count = contacts_tester.get_contacts_count()
    assert result, "Failed to get contacts count!"
    assert current_number_of_contact_records == received_contact_records_count, "Incorrect contact records number!"

    # Get all previously added contacts
    result, returned_contacts, total_count = contacts_tester.get_contacts_with_offset_and_limit(
        initial_number_of_contact_records, current_number_of_contact_records - initial_number_of_contact_records)
    assert result, "Failed to get contacts with offset and limit!"

    # This section requires to have empty phonebook
    # TO DO: fix it to work with any number of contacts in phonebook
    for record in returned_contacts:
        print(record)
        del record["id"]
    # Returned contacts are sorted by altName
    assert returned_contacts[0] == fourth_contact_record, "Received contact record is different than expected!"
    assert returned_contacts[1] == second_contact_record, "Received contact record is different than expected!"
    assert returned_contacts[2] == first_contact_record, "Received contact record is different than expected!"
    assert returned_contacts[3] == third_contact_record, "Received contact record is different than expected!"
    assert current_number_of_contact_records == total_count, "Incorrect contact records number!"

    # Get previously added contacts without 2 of them (test offset)
    result, returned_contacts, total_count = contacts_tester.get_contacts_with_offset_and_limit(
        initial_number_of_contact_records + 2, current_number_of_contact_records - initial_number_of_contact_records)
    assert result, "Failed to get contacts with offset and limit!"

    for record in returned_contacts:
        print(record)
        del record["id"]
    # Returned contacts are sorted by altName
    assert returned_contacts[0] == first_contact_record, "Received contact record is different than expected!"
    assert returned_contacts[1] == third_contact_record, "Received contact record is different than expected!"
    assert current_number_of_contact_records == total_count, "Incorrect contact records number!"

    # Get two of previously added contacts (test limit)
    result, returned_contacts, total_count = contacts_tester.get_contacts_with_offset_and_limit(
        initial_number_of_contact_records, 2)
    assert result, "Failed to get contacts with offset and limit!"

    for record in returned_contacts:
        print(record)
        del record["id"]
    # Returned contacts are sorted by altName
    assert returned_contacts[0] == fourth_contact_record, "Received contact record is different than expected!"
    assert returned_contacts[1] == second_contact_record, "Received contact record is different than expected!"
    assert current_number_of_contact_records == total_count, "Incorrect contact records number!"

    # Clean up
    assert contacts_tester.delete_contact_by_id(first_contact_id), "Failed to delete a contact!"
    assert contacts_tester.delete_contact_by_id(second_contact_id), "Failed to delete a contact!"
    assert contacts_tester.delete_contact_by_id(third_contact_id), "Failed to delete a contact!"
    assert contacts_tester.delete_contact_by_id(fourth_contact_id), "Failed to delete a contact!"
    result, received_contact_records_count = contacts_tester.get_contacts_count()
    assert result, "Failed to get contacts count!"
    assert initial_number_of_contact_records == received_contact_records_count, "Incorrect contact records number!"
