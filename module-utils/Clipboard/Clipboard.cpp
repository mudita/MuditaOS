// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "Clipboard.hpp"

cpp_freertos::MutexStandard Clipboard::mutex;

Clipboard &Clipboard::getInstance()
{
    static Clipboard clipboard;
    return clipboard;
}

void Clipboard::copy(const std::string &newData)
{
    cpp_freertos::LockGuard lock(mutex);
    data      = newData;
    validData = true;
}

std::string Clipboard::paste()
{
    cpp_freertos::LockGuard lock(mutex);
    return data;
}
