// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/TimeUtils.hpp>

#include <time/time_conversion.hpp>

namespace utils::time
{
    std::time_t getCurrentTime()
    {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    std::time_t calculateTimeDifference(std::time_t alarmTime, std::time_t currentTime)
    {
        return Duration{alarmTime, currentTime}.get();
    }

    UTF8 getBottomDescription(std::time_t timestamp)
    {
        const auto prefix   = translate("app_bellmain_home_screen_bottom_desc");
        const auto duration = Duration{timestamp};
        const auto timeText = [](time_t hours, time_t minutes) -> std::string {
            if (hours == 0 && minutes == 0) {
                return "1 min";
            }
            else if (hours == 0) {
                return std::to_string(minutes) + " min";
            }
            else if (minutes == 0) {
                return std::to_string(hours) + " h";
            }
            else {
                return std::to_string(hours) + " h & " + std::to_string(minutes) + " min";
            }
        }(duration.getHours(), duration.getMinutes());
        return UTF8("<text>" + prefix + "<br />" + translate("app_bellmain_home_screen_bottom_desc_in") + " " +
                    timeText + "</text>");
    }
} // namespace utils::time
