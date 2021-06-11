// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceCellularPriv.hpp"
#include "messages.hpp"
#include "static-api.hpp"
#include "Settings.hpp"

#include <service-cellular-api>

#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>

#include <bsp/cellular/bsp_cellular.hpp>

namespace cellular::internal
{
    using namespace cellular::msg;

    ServiceCellularPriv::ServiceCellularPriv(ServiceCellular *owner)
        : owner{owner}, simCard{std::make_unique<SimCard>()}, state{std::make_unique<State>(owner)}
    {
        initSimCard();
    }

    ServiceCellularPriv::~ServiceCellularPriv()
    {}

    void ServiceCellularPriv::startup()
    {
        /**
         * Create Settings object, which requires owner to be constructed and shared already
         */
        settings = std::make_unique<service::Settings>(owner);

        /**
         * Init static data at startup (InitHandler) stage as there is no access to Settings earlier
         */
        initStaticData();

        /**
         * Connect all handlers
         */
        connectSimCard();

        /**
         * Apply initial Settings
         */
        if (auto activeSim = settings->getSimSlot()) {
            owner->bus.sendUnicast<request::sim::SetActiveSim>(*activeSim);
        }
    }

    void ServiceCellularPriv::initStaticData()
    {
        using namespace cellular::msg;
        auto data = static_data::get();

        /**
         * Set initial values before activating on-change callbacks
         */
        data->setTrayState(bsp::cellular::sim::trayInserted() ? api::TrayState::Inserted : api::TrayState::Ejected);
        if (auto activeSim = settings->getSimSlot()) {
            data->setSimSlot(*activeSim);
        }

        /**
         * Handle changes that require handling - undefined callback are ignored by static_data
         */
        data->onSimSlot  = [this](api::SimSlot slot) { settings->setSimSlot(slot); };
        data->onSimState = [this](api::SimState state) {
            owner->bus.sendMulticast<notification::SimStateChanged>(state);
        };
    }

    void ServiceCellularPriv::initSimCard()
    {
        simCard->onNeedPin = [this](unsigned int attempts) {
            owner->bus.sendMulticast<notification::SimNeedPin>(attempts);
        };
        simCard->onNeedPuk = [this](unsigned int attempts) {
            owner->bus.sendMulticast<notification::SimNeedPuk>(attempts);
        };
        simCard->onSimBlocked   = [this]() { owner->bus.sendMulticast<notification::SimBlocked>(); };
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

} // namespace cellular::internal
