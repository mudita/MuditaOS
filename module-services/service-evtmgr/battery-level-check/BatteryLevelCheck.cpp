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
        enum class CheckState
        {
            InitialCheck,
            LevelCritical,
            LevelNormal
        };

        CheckState state = CheckState::InitialCheck;

        constexpr inline auto DEFAULT_LEVEL = 10;

        unsigned int batteryLevelCritical = DEFAULT_LEVEL;

        sys::Service *parentService = nullptr;

        bool isBatteryLevelCritical(unsigned int level)
        {
            return level < batteryLevelCritical;
        }

        void sendCriticalLevelMessage()
        {
            auto levelCriticalMessage = std::make_shared<sevm::BatteryLevelCriticalMessage>();
            sys::Bus::SendUnicast(levelCriticalMessage, service::name::system_manager, parentService);
        }

        void sendNormalLevelMessage()
        {
            auto levelNormalMessage = std::make_shared<sevm::BatteryLevelNormalMessage>();
            sys::Bus::SendUnicast(levelNormalMessage, service::name::system_manager, parentService);
        }
    } // namespace

    void init(sys::Service *service)
    {
        parentService = service;
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

    void setBatteryCriticalLevel(unsigned int level)
    {
        batteryLevelCritical = level;
        checkBatteryLevelCritical();
    }

} // namespace battery_level_check
