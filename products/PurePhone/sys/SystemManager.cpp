// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <sys/SystemManager.hpp>
#include <sys/messages/PhoneModeRequest.hpp>
#include <sys/messages/TetheringPhoneModeChangeProhibitedMessage.hpp>

#include <evtmgr/EVMessages.hpp>
#include <system/messages/SystemManagerMessage.hpp>
#include <system/messages/TetheringQuestionRequest.hpp>
#include <system/messages/TetheringStateRequest.hpp>
#include <EventStore.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-evtmgr/Constants.hpp>

namespace sys
{
    namespace
    {
        const std::map<bsp::KeyCodes, phone_modes::PhoneMode> SliderStateToPhoneModeMapping = {
            {bsp::KeyCodes::SSwitchUp, phone_modes::PhoneMode::Connected},
            {bsp::KeyCodes::SSwitchMid, phone_modes::PhoneMode::DoNotDisturb},
            {bsp::KeyCodes::SSwitchDown, phone_modes::PhoneMode::Offline}};
    } // namespace

    SystemManager::SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators)
        : SystemManagerCommon(std::move(creators))
    {}

    void SystemManager::StartSystem(InitFunction sysInit, InitFunction appSpaceInit)
    {
        phoneModeSubject = std::make_unique<phone_modes::Subject>(this);
        SystemManagerCommon::StartSystem(std::move(sysInit), std::move(appSpaceInit));
    }

    ReturnCodes SystemManager::InitHandler()
    {
        SystemManagerCommon::InitHandler();

        connect(typeid(PhoneModeRequest), [this](sys::Message *message) -> sys::MessagePointer {
            auto request = static_cast<PhoneModeRequest *>(message);
            return handlePhoneModeRequest(request);
        });

        connect(typeid(TetheringStateRequest), [this](sys::Message *message) -> sys::MessagePointer {
            auto request = static_cast<TetheringStateRequest *>(message);
            return handleTetheringStateRequest(request);
        });

        connect(typeid(TetheringEnabledResponse), [this](sys::Message *message) -> sys::MessagePointer {
            auto response = static_cast<TetheringEnabledResponse *>(message);
            return enableTethering(response);
        });

        connect(CellularCheckIfStartAllowedMessage(), [&](Message *) {
            switch (Store::Battery::get().levelState) {
            case Store::Battery::LevelState::Normal:
                CellularServiceAPI::ChangeModulePowerState(this, cellular::service::State::PowerState::On);
                break;
            case Store::Battery::LevelState::CriticalCharging:
                [[fallthrough]];
            case Store::Battery::LevelState::CriticalNotCharging:
                CellularServiceAPI::ChangeModulePowerState(this, cellular::service::State::PowerState::Off);
                break;
            case Store::Battery::LevelState::Shutdown:
                break;
            }
            return MessageNone{};
        });

        return ReturnCodes::Success;
    }

    phone_modes::PhoneMode SystemManager::translateSliderState(const RawKey &key)
    {
        const auto code = key.keyCode;
        if (code != bsp::KeyCodes::SSwitchUp && code != bsp::KeyCodes::SSwitchMid &&
            code != bsp::KeyCodes::SSwitchDown) {
            throw std::invalid_argument{"Invalid key code passed."};
        }
        return SliderStateToPhoneModeMapping.at(code);
    }

    MessagePointer SystemManager::handlePhoneModeRequest(PhoneModeRequest *request)
    {
        LOG_INFO("Phone mode change requested.");
        if (phoneModeSubject->isTetheringEnabled()) {
            LOG_WARN("Changing phone mode when tethering is enabled!");
            // display popup
            bus.sendUnicast(std::make_shared<TetheringPhoneModeChangeProhibitedMessage>(), service::name::appmgr);
            return MessageNone{};
        }
        phoneModeSubject->setPhoneMode(request->getPhoneMode());
        return MessageNone{};
    }

    MessagePointer SystemManager::handleTetheringStateRequest(TetheringStateRequest *request)
    {
        LOG_INFO("Tethering state change requested");

        if (Store::Battery::get().levelState != Store::Battery::LevelState::Normal) {
            LOG_INFO("Tethering state change refused - battery too low");
            return MessageNone{};
        }

        if (const auto requestedState = request->getTetheringState(); requestedState == phone_modes::Tethering::On) {
            if (phoneModeSubject->isTetheringPossible()) {
                bus.sendUnicast(std::make_shared<TetheringQuestionRequest>(), service::name::appmgr);
            }
        }
        else {
            if (const auto tetheringChanged = phoneModeSubject->setTetheringMode(phone_modes::Tethering::Off);
                !tetheringChanged) {
                bus.sendUnicast(std::make_shared<TetheringQuestionAbort>(), service::name::appmgr);
            }
            else {
                // Turned on, disabling...
                LOG_INFO("Disabling tethering");
                bus.sendUnicast(std::make_shared<sevm::RequestPhoneModeForceUpdate>(), service::name::evt_manager);
            }
        }
        return MessageNone{};
    }

    MessagePointer SystemManager::enableTethering([[maybe_unused]] TetheringEnabledResponse *response)
    {
        phoneModeSubject->setTetheringMode(phone_modes::Tethering::On);
        return MessageNone{};
    }

    void SystemManager::batteryNormalLevelAction()
    {
        SystemManagerCommon::batteryNormalLevelAction();
        CellularServiceAPI::ChangeModulePowerState(this, cellular::service::State::PowerState::On);
        auto battNormalMsg = std::make_shared<CriticalBatteryLevelNotification>(false);
        bus.sendUnicast(std::move(battNormalMsg), service::name::appmgr);
    }

    void SystemManager::batteryCriticalLevelAction(bool charging)
    {
        SystemManagerCommon::batteryCriticalLevelAction(charging);
        CellularServiceAPI::ChangeModulePowerState(this, cellular::service::State::PowerState::Off);
        auto msg = std::make_shared<CriticalBatteryLevelNotification>(true, charging);
        bus.sendUnicast(std::move(msg), service::name::appmgr);
    }
} // namespace sys
