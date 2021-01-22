// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StringCircularBuffer.hpp"

std::pair<bool, std::string> StringCircularBuffer::get()
{
    if (isEmpty()) {
        return {false, ""};
    }

    const std::string val = buffer[tail];
    full                  = false;
    tail                  = (tail + 1) % capacity;
    --size;

    return {true, val};
}

void StringCircularBuffer::put(const std::string &item)
{
    updateMembersBeforePut();
    buffer[head] = item;
    updateMembersAfterPut();
}

void StringCircularBuffer::put(std::string &&item)
{
    updateMembersBeforePut();
    buffer[head] = std::move(item);
    updateMembersAfterPut();
}

void StringCircularBuffer::reset()
{
    head = tail;
    full = false;
    size = 0;
}

void StringCircularBuffer::updateMembersAfterPut()
{
    head = (head + 1) % capacity;
    full = head == tail;
}

void StringCircularBuffer::updateMembersBeforePut()
{
    if (full) {
        tail = (tail + 1) % capacity;
    }
    else {
        ++size;
    }
}
