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
    class Multicast
    {
      public:
        virtual void notifyIncomingCall()                                         = 0;
        virtual void notifyIdentifiedCall(const utils::PhoneNumber::View &number) = 0;
        virtual void notifyCallActive()                                           = 0;
        virtual void notifyCallAborted()                                          = 0;
        virtual void notifyOutgoingCallAnswered()                                              = 0;
        virtual void notifyCallStarted(const utils::PhoneNumber &number, const CallType &type) = 0;
        virtual void notifyCallEnded()                                                         = 0;
        virtual void notifyCallDurationUpdate(const time_t &duration)                          = 0;

        virtual ~Multicast()                                                      = default;
    };
} // namespace call::api

class CallMulticast : public call::api::Multicast
{
    sys::Service *owner;

  public:
    explicit CallMulticast(sys::Service *owner) : owner(owner)
    {}
    void notifyIncomingCall() override;
    void notifyIdentifiedCall(const utils::PhoneNumber::View &number) override;
    void notifyCallActive() override;
    void notifyCallAborted() override;
    void notifyOutgoingCallAnswered() override;
    void notifyCallStarted(const utils::PhoneNumber &number, const CallType &type) override;
    void notifyCallEnded() override;
    void notifyCallDurationUpdate(const time_t &duration) override;
};
