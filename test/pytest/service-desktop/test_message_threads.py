# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.request import TransactionError
from harness.api.messages import GetThreadsWithOffsetAndLimit, MarkThreadAsUnread, GetMessageById, AddMessage, DeleteMessageById


class ThreadsTester:
    def __init__(self, harness):
        self.harness = harness

    def get_threads_with_offset_and_limit(self, offset, limit):
        try:
            result = GetThreadsWithOffsetAndLimit(offset, limit).run(self.harness)
        except TransactionError:
            return False
        else:
            return True, result

    def mark_thread_as_unread(self, thread_id, set_thread_unread):
        try:
            result = MarkThreadAsUnread(thread_id, set_thread_unread).run(self.harness)
        except TransactionError:
            return False
        else:
            return True, result

    def get_message_by_id(self, message_record_id):
        try:
            result = GetMessageById(message_record_id).run(self.harness)
        except TransactionError:
            return False
        else:
            return True, result.message

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


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_marking_thread_as_read(harness):
    message_number = "123456789"
    message_body = "Test message nr 1"

    threads_tester = ThreadsTester(harness)

    result, message_record = threads_tester.add_message(message_number, message_body)
    assert result, "Failed to add message!"
    result, received_message_record = threads_tester.get_message_by_id(message_record["messageID"])
    assert result, "Failed to get message by id!"

    assert threads_tester.mark_thread_as_unread(received_message_record["threadID"], False)
    assert threads_tester.mark_thread_as_unread(received_message_record["threadID"], True)

    assert threads_tester.delete_message_by_id(message_record["messageID"]), "Failed to delete a message!"


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_getting_threads(harness):
    THREADS_PAGE_SIZE = 4
    message_numbers = ["123456789", "223456789", "323456789", "423456789", "523456789", "623456789"]
    message_body = "Test message"

    threads_tester = ThreadsTester(harness)

    for number in message_numbers:
        result = threads_tester.add_message(number, message_body)
        assert result, "Failed to add message!"

    result, response = threads_tester.get_threads_with_offset_and_limit(
        0, 0)
    assert result, "Failed to get all threads!"
    total_count_of_threads = response.totalCount
    result, response = threads_tester.get_threads_with_offset_and_limit(
        total_count_of_threads - 6, 6)
    assert result, "Failed to get threads with offset and limit!"
    assert len(response.threads) == THREADS_PAGE_SIZE
    assert response.nextPage["limit"] == 2
    assert response.nextPage["offset"] == total_count_of_threads - 2
    received_threads = response.threads
    result, response = threads_tester.get_threads_with_offset_and_limit(
        response.nextPage["offset"], response.nextPage["limit"])
    assert result, "Failed to get threads with offset and limit!"
    received_threads += response.threads
    assert len(received_threads) == 6

    for thread in received_threads:
        assert type(thread["isUnread"]) == bool
        assert type(thread["lastUpdatedAt"]) == int
        assert type(thread["messageCount"]) == int
        assert type(thread["messageSnippet"]) == str
        assert type(thread["messageType"]) == int
        assert type(thread["number"]) == str
        assert type(thread["threadID"]) == int
        message_types = [1, 2, 4, 8, 16, 18, 255]
        assert thread["messageType"] in message_types
