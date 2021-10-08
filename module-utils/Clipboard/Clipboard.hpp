// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <mutex.hpp>

#include <string>

class Clipboard
{
    Clipboard() = default;

    std::string data;
    bool validData = false;

    static cpp_freertos::MutexStandard mutex;

  public:
    Clipboard(const Clipboard &) = delete;
    Clipboard &operator=(const Clipboard &) = delete;

    static Clipboard &getInstance();

    void copy(const std::string &);
    std::string paste();

    bool gotData() const
    {
        return validData;
    }
};
