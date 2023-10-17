// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace utils
{
    namespace time
    {
        inline constexpr auto secondsInMinute        = 60;
        inline constexpr auto minutesInHour          = 60;
        inline constexpr auto hoursInday             = 24;
        inline constexpr auto minutesInQuarterOfHour = 15;
        inline constexpr auto secondsInHour          = minutesInHour * secondsInMinute;
        inline constexpr auto secondsInDay           = hoursInday * secondsInHour;
        inline constexpr auto milisecondsInSecond    = 1000;
    } // namespace time
} // namespace utils
