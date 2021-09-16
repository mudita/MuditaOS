// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/Popups.hpp"

#include <service-appmgr/Actions.hpp>

#include <string>

namespace gui
{
    enum class AlarmPopupType
    {
        AlarmRing,
        SnoozeCheck
    };

    class AlarmPopupParams : public app::manager::actions::ActionParams
    {
      public:
        explicit AlarmPopupParams(AlarmPopupType type, std::string alarmTime, std::string snoozeTime = std::string{})
            : type{type}, alarmTime{alarmTime}, snoozeTime{snoozeTime}
        {}

        [[nodiscard]] AlarmPopupType getPopupType() const
        {
            return type;
        }

        [[nodiscard]] std::string getAlarmTimeString() const
        {
            return alarmTime;
        }

        [[nodiscard]] std::string getSnoozeTimeString() const
        {
            return snoozeTime;
        }

      private:
        const AlarmPopupType type;
        const std::string alarmTime;
        const std::string snoozeTime;
    };

} // namespace gui
