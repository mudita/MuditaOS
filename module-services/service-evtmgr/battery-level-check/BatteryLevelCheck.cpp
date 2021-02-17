// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryLevelCheck.hpp"
#include "service-evtmgr/BatteryMessages.hpp"
#include "SystemManager/Constants.hpp"
#include <agents/settings/SystemSettings.hpp>
#include <common_data/EventStore.hpp>
#include <Utils.hpp>

namespace battery_level_check
{
    namespace
    {
        enum class CheckState
        {
            InitialCheck,
            LevelCritical,
            LevelNormal
        };

        CheckState state = CheckState::InitialCheck;

        unsigned int batteryLevelCritical = 0;

        sys::Service *parentService = nullptr;

        std::shared_ptr<settings::Settings> settings = nullptr;

        bool isBatteryLevelCritical(unsigned int level)
        {
            return level < batteryLevelCritical;
        }

        void sendCriticalLevelMessage()
        {
            auto levelCriticalMessage = std::make_shared<sevm::BatteryLevelCriticalMessage>();
            parentService->bus.sendUnicast(levelCriticalMessage, service::name::system_manager);
        }

        void sendNormalLevelMessage()
        {
            auto levelNormalMessage = std::make_shared<sevm::BatteryLevelNormalMessage>();
            parentService->bus.sendUnicast(levelNormalMessage, service::name::system_manager);
        }
    } // namespace

    void init(sys::Service *service, std::shared_ptr<settings::Settings> &setts)
    {
        parentService = service;
        settings      = setts;
        settings->registerValueChange(
            settings::Battery::batteryCriticalLevel,
            [&](const std::string &value) { batteryLevelCritical = utils::getNumericValue<unsigned int>(value); },
            settings::SettingsScope::Global);
    }

    void deinit()
    {
        settings->unregisterValueChange(settings::Battery::batteryCriticalLevel, settings::SettingsScope::Global);
        settings.reset();
    }

    void checkBatteryLevelCritical()
    {
        switch (state) {
        case CheckState::InitialCheck:
            if (isBatteryLevelCritical(Store::Battery::get().level)) {
                sendCriticalLevelMessage();
                state = CheckState::LevelCritical;
            }
            else {
                sendNormalLevelMessage();
                state = CheckState::LevelNormal;
            }
            break;
        case CheckState::LevelCritical:
            if (!isBatteryLevelCritical(Store::Battery::get().level)) {
                sendNormalLevelMessage();
                state = CheckState::LevelNormal;
            }
            break;
        case CheckState::LevelNormal:
            if (isBatteryLevelCritical(Store::Battery::get().level)) {
                sendCriticalLevelMessage();
                state = CheckState::LevelCritical;
            }
            break;
        }
    }

    void checkBatteryLevelCriticalWithConfirmation()
    {
        state = CheckState::InitialCheck;
        checkBatteryLevelCritical();
    }

    void setBatteryCriticalLevel(unsigned int level)
    {
        batteryLevelCritical = level;
        settings->setValue(
            settings::Battery::batteryCriticalLevel, utils::to_string(level), settings::SettingsScope::Global);
        checkBatteryLevelCritical();
    }

} // namespace battery_level_check
