// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>

namespace sys
{
    class Service;
}

class CallMulticast
{
    sys::Service &owner;

  public:
    explicit CallMulticast(sys::Service &owner) : owner(owner)
    {}
    void notifyIncommingCall();
    void notifyIdentifiedCall(const utils::PhoneNumber::View &number);
};
