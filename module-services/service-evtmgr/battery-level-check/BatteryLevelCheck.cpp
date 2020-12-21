// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryLevelCheck.hpp"
#include "service-evtmgr/BatteryMessages.hpp"
#include "SystemManager/Constants.hpp"

#include <Service/Bus.hpp>
#include <common_data/EventStore.hpp>

namespace battery_level_check
{
    namespace
    {
        constexpr inline auto DEFAULT_LEVEL = 10;

        unsigned int batteryLevelCritical = DEFAULT_LEVEL;

        sys::Service *parentService = nullptr;

        bool isBatteryLevelCritical(unsigned int level)
        {
            return level < batteryLevelCritical;
        }

    } // namespace

    void init(sys::Service *service)
    {
        parentService = service;
        checkBatteryLevelCritical();
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
        checkBatteryLevelCritical();
    }

} // namespace battery_level_check
