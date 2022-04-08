// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallMulticast.hpp"
#include "service-cellular/CellularMessage.hpp"
#include <Service/Service.hpp>

void CallMulticast::notifyIncommingCall()
{
    owner.bus.sendMulticast(std::make_shared<CellularIncominCallMessage>(),
                            sys::BusChannel::ServiceCellularNotifications);
}

void CallMulticast::notifyIdentifiedCall(const utils::PhoneNumber::View &number)
{
    owner.bus.sendMulticast(std::make_shared<CellularCallerIdMessage>(number),
                            sys::BusChannel::ServiceCellularNotifications);
}
