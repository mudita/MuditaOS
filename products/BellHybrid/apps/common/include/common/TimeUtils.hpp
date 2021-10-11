// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <utf8/UTF8.hpp>

#include <ctime>

namespace utils::time
{
    std::time_t getCurrentTime();
    std::time_t calculateTimeDifference(std::time_t alarmTime, std::time_t currentTime);
    std::time_t calculateMinutesDifference(std::time_t alarmTime, std::time_t currentTime);
    UTF8 getBottomDescription(std::time_t timestamp);
} // namespace utils::time
