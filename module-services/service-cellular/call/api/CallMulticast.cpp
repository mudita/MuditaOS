// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallMulticast.hpp"
#include "service-cellular/CellularMessage.hpp"

void CallMulticast::notifyIncomingCall()
{
    owner->bus.sendMulticast(std::make_shared<cellular::IncomingCallMessage>(),
                             sys::BusChannel::ServiceCellularNotifications);
}

void CallMulticast::notifyIdentifiedCall(const utils::PhoneNumber::View &number)
{
    owner->bus.sendMulticast(std::make_shared<cellular::CallerIdMessage>(number),
                             sys::BusChannel::ServiceCellularNotifications);
}

void CallMulticast::notifyCallAborted()
{
    owner->bus.sendMulticast(std::make_shared<cellular::CallAbortedNotification>(),
                             sys::BusChannel::ServiceCellularNotifications);
}
void CallMulticast::notifyOutgoingCallAnswered()
{
    owner->bus.sendMulticast(std::make_shared<cellular::CallOutgoingAccepted>(),
                             sys::BusChannel::ServiceCellularNotifications);
}

void CallMulticast::notifyCallStarted(const utils::PhoneNumber &number, const CallType &type)
{
    owner->bus.sendMulticast(std::make_shared<cellular::CallStartedNotification>(number, type == CallType::CT_INCOMING),
                             sys::BusChannel::ServiceCellularNotifications);
}

void CallMulticast::notifyCallEnded()
{
    owner->bus.sendMulticast(std::make_shared<cellular::CallEndedNotification>(),
                             sys::BusChannel::ServiceCellularNotifications);
}

void CallMulticast::notifyCallActive()
{
    owner->bus.sendMulticast(std::make_shared<cellular::CallActiveNotification>(),
                             sys::BusChannel::ServiceCellularNotifications);
}

void CallMulticast::notifyCallDurationUpdate(const time_t &duration)
{
    owner->bus.sendMulticast(std::make_shared<cellular::CallDurationNotification>(duration),
                             sys::BusChannel::ServiceCellularNotifications);
}
