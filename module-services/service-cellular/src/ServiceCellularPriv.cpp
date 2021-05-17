// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceCellularPriv.hpp"
#include "messages.hpp"

#include <service-cellular-api>

#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>

namespace cellular::internal
{
    ServiceCellularPriv::ServiceCellularPriv(ServiceCellular *owner)
        : owner{owner}, simCard{std::make_unique<SimCard>()}, state{std::make_unique<State>(owner)}
    {
        initSimCard();
    }

    void ServiceCellularPriv::initSimCard()
    {
        using namespace cellular::msg;
        simCard->onSimReady = [this](bool ready) {
            // SIM causes SIM INIT, only on ready
            if (ready) {
                state->set(State::ST::SimInit);
            }
            owner->bus.sendMulticast(std::make_shared<notification::SimReady>(ready),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
        simCard->onNeedPin = [this](unsigned int attempts) {
            owner->bus.sendMulticast(std::make_shared<notification::SimNeedPin>(attempts),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
        simCard->onNeedPuk = [this](unsigned int attempts) {
            owner->bus.sendMulticast(std::make_shared<notification::SimNeedPuk>(attempts),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
        simCard->onSimBlocked = [this]() {
            owner->bus.sendMulticast(std::make_shared<notification::SimBlocked>(),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
        simCard->onSimEvent = [this]() {
            owner->bus.sendUnicast(std::make_shared<sevm::SIMMessage>(), ::service::name::evt_manager);
        };
        simCard->onUnhandledCME = [this](unsigned int code) {
            owner->bus.sendMulticast(std::make_shared<notification::UnhandledCME>(code),
                                     sys::BusChannel::ServiceCellularNotifications);
        };
    }

    void ServiceCellularPriv::connectSimCard()
    {
        using namespace cellular::msg;
        owner->connect(typeid(request::sim::SetActiveSim), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::SetActiveSim *>(request);
            return std::make_shared<request::sim::SetActiveSim::Response>(simCard->handleSetActiveSim(msg->sim));
        });
        owner->connect(typeid(request::sim::GetLockState), [&](sys::Message *) -> sys::MessagePointer {
            return std::make_shared<request::sim::GetLockState::Response>(simCard->handleIsPinLocked());
        });
        owner->connect(typeid(request::sim::ChangePin), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::ChangePin *>(request);
            return std::make_shared<request::sim::ChangePin::Response>(simCard->handleChangePin(msg->oldPin, msg->pin));
        });
        owner->connect(typeid(request::sim::UnblockWithPuk), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::UnblockWithPuk *>(request);
            return std::make_shared<request::sim::UnblockWithPuk::Response>(
                simCard->handleUnblockWithPuk(msg->puk, msg->pin));
        });
        owner->connect(typeid(request::sim::SetPinLock), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::SetPinLock *>(request);
            return std::make_shared<request::sim::SetPinLock::Response>(simCard->handleSetPinLock(msg->pin, msg->lock),
                                                                        msg->lock);
        });
        owner->connect(typeid(request::sim::PinUnlock), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::PinUnlock *>(request);
            return std::make_shared<request::sim::PinUnlock::Response>(simCard->handlePinUnlock(msg->pin));
        });
        owner->connect(typeid(internal::msg::SimStateChanged), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<internal::msg::SimStateChanged *>(request);
            simCard->handleSimStateChanged(msg->state);
            return sys::MessageNone{};
        });
    }

} // namespace cellular::internal
