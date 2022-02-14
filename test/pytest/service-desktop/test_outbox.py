# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import time
import pytest
from harness.api.contacts import AddContact, DeleteContactById
from harness.api.messages import AddMessage, DeleteMessageById
from harness.api.outbox import NotificationEntry, NotificationType, NotificationChange, GetNotifications, \
    DeleteNotifications
from harness.request import TransactionError


class OutboxTester:
    def __init__(self, harness):
        self.harness = harness

    def get_notifications(self):
        try:
            notifications = GetNotifications().run(self.harness).entries
        except TransactionError:
            return False
        else:
            return True, notifications

    def delete_notifications(self, entries: list):
        try:
            DeleteNotifications(entries).run(self.harness)
        except TransactionError:
            return False
        else:
            return True

    def add_message(self, message_number, message_body):
        try:
            message = AddMessage(message_number, message_body).run(self.harness).message
        except TransactionError:
            return False
        else:
            return True, message

    def delete_message_by_id(self, message_record_id):
        try:
            DeleteMessageById(message_record_id).run(self.harness)
        except TransactionError:
            return False
        else:
            return True

    def add_contact(self, contact_record):
        try:
            contact_id = AddContact(contact_record).run(self.harness).id
        except TransactionError:
            return False
        else:
            return True, contact_id

    def delete_contact_by_id(self, contact_record_id):
        try:
            DeleteContactById(contact_record_id).run(self.harness)
        except TransactionError:
            return False
        else:
            return True


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_getting_notifications(harness):
    outbox_tester = OutboxTester(harness)
    result, received_notifications = outbox_tester.get_notifications()
    assert result, "Failed to get notifications!"
    assert not received_notifications, "Notification list is not empty at the beginning of the test!"

    # Add message to generate "message created" and "thread created" notifications
    message_number = "123456789"
    message_body = "Hello, how are you?"

    result, message_record = outbox_tester.add_message(message_number, message_body)
    assert result, "Failed to add message!"

    # Add contact to generate "contact created" notification
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

    result, contact_id = outbox_tester.add_contact(contact_record)
    assert result, "Failed to add contact!"

    result, received_notifications = outbox_tester.get_notifications()
    assert result, "Failed to get notifications!"

    # Check if all notifications are present
    found_message_notification = False
    for notification in received_notifications:
        if notification.type == NotificationType.MESSAGE and notification.change == NotificationChange.CREATED \
                and notification.record_id == message_record["messageID"]:
            found_message_notification = True
            break
    found_thread_notification = False
    for notification in received_notifications:
        if notification.type == NotificationType.THREAD and notification.change == NotificationChange.CREATED:
            found_thread_notification = True
            break
    found_contacts_notification = False
    for notification in received_notifications:
        if notification.type == NotificationType.CONTACT and notification.change == NotificationChange.CREATED \
                and notification.record_id == contact_id:
            found_contacts_notification = True
            break
    assert found_message_notification and found_thread_notification and found_contacts_notification

    # Wait for 20 seconds to be sure that notifications are cleaned by timer
    time.sleep(20)

    result, received_notifications = outbox_tester.get_notifications()
    assert result, "Failed to get notifications!"
    assert not received_notifications, "Notification list is not empty after timeout!"

    assert outbox_tester.delete_message_by_id(message_record["messageID"]), "Failed to delete a message!"
    assert outbox_tester.delete_contact_by_id(contact_id), "Failed to delete a contact!"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_deleting_notifications(harness):
    outbox_tester = OutboxTester(harness)
    result, received_notifications = outbox_tester.get_notifications()
    assert result, "Failed to get notifications!"
    assert not received_notifications, "Notification list is not empty at the beginning of the test!"

    # Add message to generate "message created" and "thread created" notifications
    message_number = "123456789"
    message_body = "Hello, how are you?"

    result, message_record = outbox_tester.add_message(message_number, message_body)
    assert result, "Failed to add message!"

    # Add contact to generate "contact created" notification
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

    result, contact_id = outbox_tester.add_contact(contact_record)
    assert result, "Failed to add contact!"

    result, received_notifications = outbox_tester.get_notifications()
    assert result, "Failed to get notifications!"
    uids_of_notifications_to_be_deleted = []
    for notification in received_notifications:
        uids_of_notifications_to_be_deleted.append(notification.uid)

    result = outbox_tester.delete_notifications(uids_of_notifications_to_be_deleted)
    assert result, "Failed to get notifications!"

    # Check if all generated notifications are deleted
    result, received_notifications = outbox_tester.get_notifications()
    assert result, "Failed to get notifications!"

    for notification in received_notifications:
        assert notification.uid not in uids_of_notifications_to_be_deleted, "Notification not deleted!"

    assert outbox_tester.delete_message_by_id(message_record["messageID"]), "Failed to delete a message!"
    assert outbox_tester.delete_contact_by_id(contact_id), "Failed to delete a contact!"
