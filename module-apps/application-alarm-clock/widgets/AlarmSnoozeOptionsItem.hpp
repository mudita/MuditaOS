// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmOptionsItem.hpp"

namespace gui
{
    class AlarmSnoozeOptionsItem : public AlarmOptionsItem
    {
      public:
        explicit AlarmSnoozeOptionsItem(const std::string &description);

      private:
        std::vector<std::pair<std::chrono::minutes, const std::string>> snoozeOptions = {
            {std::chrono::minutes{0}, utils::translate("app_alarm_clock_no_snooze")},
            {std::chrono::minutes{5}, utils::translate("app_alarm_clock_snooze_5_min")},
            {std::chrono::minutes{10}, utils::translate("app_alarm_clock_snooze_10_min")},
            {std::chrono::minutes{15}, utils::translate("app_alarm_clock_snooze_15_min")},
            {std::chrono::minutes{30}, utils::translate("app_alarm_clock_snooze_30_min")}};
    };
} /* namespace gui */
