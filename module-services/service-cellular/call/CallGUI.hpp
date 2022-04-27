// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>
#include <Tables/CalllogTable.hpp>

namespace sys
{
    class Service;
}

class CallGUI
{
    struct CallMeta;
    sys::Service &owner;

  public:
    explicit CallGUI(sys::Service &);

    void notifyRING();
    void notifyCLIP(const utils::PhoneNumber::View &number);
    void notifyCallStarted(utils::PhoneNumber phoneNumber, const CallType type);
    void notifyCallEnded();
    void notifyCallActive();
    void notifyCallDurationUpdate(const time_t &duration);
};
