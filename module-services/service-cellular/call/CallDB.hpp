// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>

namespace sys
{
    class Service;
}

class CallDB
{
    sys::Service &owner;

  public:
    explicit CallDB(sys::Service &);

    void incrementNotAnsweredCallsNotification(const utils::PhoneNumber::View &number);
};
