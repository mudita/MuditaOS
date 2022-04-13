// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>
#include <Tables/CalllogTable.hpp>

namespace sys
{
    class Service;
}

namespace call::api
{
    class GUI
    {
      public:
        virtual void notifyRING()                                                              = 0;
        virtual void notifyCLIP(const utils::PhoneNumber::View &number)                        = 0;
        virtual void notifyCallStarted(const utils::PhoneNumber &number, const CallType &type) = 0;
        virtual void notifyCallEnded()                                                         = 0;
        virtual void notifyCallActive()                                                        = 0;
        virtual void notifyCallDurationUpdate(const time_t &duration)                          = 0;
        virtual ~GUI()                                                                         = default;
    };
}; // namespace call::api

class CallGUI : public call::api::GUI
{
    struct CallMeta;
    sys::Service *owner;

  public:
    explicit CallGUI(sys::Service *);

    void notifyRING() override;
    void notifyCLIP(const utils::PhoneNumber::View &number) override;
    void notifyCallStarted(const utils::PhoneNumber &number, const CallType &type) override;
    void notifyCallEnded() override;
    void notifyCallActive() override;
    void notifyCallDurationUpdate(const time_t &duration) override;
};
