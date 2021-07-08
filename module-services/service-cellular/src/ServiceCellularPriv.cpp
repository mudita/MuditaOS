// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceCellularPriv.hpp"
#include "messages.hpp"

#include <service-cellular-api>

#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>

#include <service-time/service-time/TimeMessage.hpp>
#include <service-time/Constants.hpp>

using service::name::service_time;

namespace cellular::internal
{
    ServiceCellularPriv::ServiceCellularPriv(ServiceCellular *owner)
        : owner{owner}, simCard{std::make_unique<SimCard>()}, state{std::make_unique<State>(owner)},
          networkTime{std::make_unique<NetworkTime>()}
    {
        initSimCard();
    }

    void ServiceCellularPriv::initSimCard()
    {
        using namespace cellular::msg;
        simCard->onSimReady = [this]() {
            state->set(State::ST::Ready);
            owner->bus.sendMulticast<notification::SimReady>();
        };
        simCard->onNeedPin = [this](unsigned int attempts) {
            owner->bus.sendMulticast<notification::SimNeedPin>(attempts);
        };
        simCard->onNeedPuk = [this](unsigned int attempts) {
            owner->bus.sendMulticast<notification::SimNeedPuk>(attempts);
        };
        simCard->onSimBlocked   = [this]() { owner->bus.sendMulticast<notification::SimBlocked>(); };
        simCard->onSimEvent     = [this]() { owner->bus.sendMulticast<notification::SimStateUpdate>(); };
        simCard->onUnhandledCME = [this](unsigned int code) {
            owner->bus.sendMulticast<notification::UnhandledCME>(code);
        };
    }

    void ServiceCellularPriv::connectSimCard()
    {
        using namespace cellular::msg;
        /**
         * Request message handlers
         */
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

        /**
         * Notification message handlers
         */
        owner->connect(typeid(sevm::SIMTrayMessage), [&](sys::Message *request) -> sys::MessagePointer {
            simCard->handleTrayState();
            return sys::MessageNone{};
        });

        /**
         * Internal message handlers
         */
        owner->connect(typeid(internal::msg::HandleATSimStateChange),
                       [&](sys::Message *request) -> sys::MessagePointer {
                           auto msg = static_cast<internal::msg::HandleATSimStateChange *>(request);
                           simCard->handleATSimStateChange(msg->state);
                           return sys::MessageNone{};
                       });
    }

    void ServiceCellularPriv::connectNetworkTime()
    {
        owner->connect(typeid(stm::message::AutomaticDateAndTimeChangedMessage),
                       [&](sys::Message *request) -> sys::MessagePointer {
                           auto message = static_cast<stm::message::AutomaticDateAndTimeChangedMessage *>(request);
                           networkTime->processSettings(message->getValue());
                           return sys::MessageNone{};
                       });
        owner->connect(typeid(stm::message::GetAutomaticDateAndTimeResponse),
                       [&](sys::Message *request) -> sys::MessagePointer {
                           auto message = static_cast<stm::message::GetAutomaticDateAndTimeResponse *>(request);
                           networkTime->processSettings(message->isAutomaticDateAndTime());
                           return sys::MessageNone{};
                       });
    }

    void ServiceCellularPriv::requestNetworkTimeSettings()
    {
        owner->bus.sendUnicast(networkTime->createSettingsRequest(), service_time);
    }

} // namespace cellular::internal
