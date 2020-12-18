// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryLevelCheck.hpp"
#include "service-evtmgr/BatteryMessages.hpp"
#include "SystemManager/Constants.hpp"

#include <Service/Bus.hpp>
#include <common_data/EventStore.hpp>
#include <service-db/service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <module-utils/Utils.hpp>

namespace battery_level_check
{
    namespace
    {
        constexpr inline auto DEFAULT_LEVEL = 10;
        unsigned int batteryLevelCritical   = DEFAULT_LEVEL;

        sys::Service *parentService = nullptr;

        std::unique_ptr<settings::Settings> settings;

        bool isBatteryLevelCritical(unsigned int level)
        {
            return level < batteryLevelCritical;
        }

        void batteryLevelSettingsCallback(std::string setting)
        {
            if (!setting.empty()) {
                batteryLevelCritical = utils::getNumericValue<unsigned int>(setting);
            }
            else {
                setBatteryCriticalLevel(DEFAULT_LEVEL);
            }
        }
    } // namespace

    void init(sys::Service *service)
    {
        parentService = service;
        settings      = std::make_unique<settings::Settings>(service);
        settings->registerValueChange(settings::Battery::criticalLevel,
                                      [&](std::string value) { batteryLevelSettingsCallback(value); });
    }

    void deinit()
    {
        settings->unregisterValueChange(settings::Battery::criticalLevel);
        settings.reset();
    }

    void checkBatteryLevelCritical()
    {
        if (Store::Battery::get().state == Store::Battery::State::Discharging) {
            if (isBatteryLevelCritical(Store::Battery::get().level)) {
                auto levelCriticalMessage = std::make_shared<sevm::BatteryLevelCriticalMessage>();
                sys::Bus::SendUnicast(levelCriticalMessage, service::name::system_manager, parentService);
            }
        }
    }

    void setBatteryCriticalLevel(unsigned int level)
    {
        batteryLevelCritical = level;
        settings->setValue(settings::Battery::criticalLevel, std::to_string(batteryLevelCritical));
        checkBatteryLevelCritical();
    }

} // namespace battery_level_check
