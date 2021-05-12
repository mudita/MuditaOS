// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceCellularPriv.hpp"

#include <service-cellular-api>

#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>

namespace cellular::internal
{
    ServiceCellularPriv::ServiceCellularPriv(ServiceCellular *owner)
        : owner{owner}, simCard{new SimCard(owner)}, state{new State(owner)}
    {
        initSimCard();
    }

    void ServiceCellularPriv::initSimCard()
    {
        simCard->onSimReady = [this](bool ready) {
            // SIM causes SIM INIT, only on ready
            if (ready) {
                state->set(State::ST::SimInit);
            }
            owner->bus.sendMulticast(std::make_shared<msg::notification::SimReady>(ready),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
        simCard->onNeedPin = [this](unsigned int attempts) {
            owner->bus.sendMulticast(std::make_shared<msg::notification::SimNeedPin>(attempts),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
        simCard->onNeedPuk = [this](unsigned int attempts) {
            owner->bus.sendMulticast(std::make_shared<msg::notification::SimNeedPuk>(attempts),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
        simCard->onSimBlocked = [this]() {
            owner->bus.sendMulticast(std::make_shared<msg::notification::SimBlocked>(),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
        simCard->onSimEvent = [this]() {
            owner->bus.sendUnicast(std::make_shared<sevm::SIMMessage>(), ::service::name::evt_manager);
        };
        simCard->onUnhandledCME = [this](unsigned int code) {
            owner->bus.sendMulticast(std::make_shared<msg::notification::UnhandledCME>(code),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
    }

} // namespace cellular::internal
