// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-time/TimezoneHandler.hpp"
#include <time/time_constants.hpp>
#include <iomanip>

auto TimezoneHandler::getTimezone() -> std::string const
{
    auto constexpr maximumTimezoneOffset = 11;
    auto sign                            = offset.count() >= 0 ? "-" : "+";

    auto hours   = abs(offset.count() / utils::time::minutesInHour);
    auto minutes = abs(offset.count() % utils::time::minutesInHour);

    if (hours > maximumTimezoneOffset || hours < -maximumTimezoneOffset) {
        hours   = 0;
        minutes = 0;
    }
    std::stringstream stream;
    stream << "MUD" << sign << hours << ":" << std::setw(2) << std::setfill('0') << minutes;

    return stream.str();
}
