// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceCellularPriv.hpp"

#include <service-cellular-api>

namespace cellular::internal
{
    ServiceCellularPriv::ServiceCellularPriv(ServiceCellular *owner)
        : owner{owner}, simCard{new SimCard(owner)}, state{new State(owner)}
    {
        initSimCard();
    }

    void ServiceCellularPriv::initSimCard()
    {
        simCard->onSimReady     = [this]() { state->set(State::ST::SimInit); };
        simCard->onUnhandledCME = [this](unsigned int code) {
            owner->bus.sendMulticast(std::make_shared<msg::notification::UnhandledCME>(code),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
    }

} // namespace cellular::internal
