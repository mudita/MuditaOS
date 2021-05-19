// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-time/TimeManager.hpp>
#include <service-time/TimezoneHandler.hpp>

#include <chrono>
#include "time.h"

void TimeManager::handleCellularTimeUpdate(struct tm time, std::chrono::minutes timezoneOffset)
{
    auto timezone = TimezoneHandler(timezoneOffset).getTimezone();
    rtcCommand->setTime(time);
    rtcCommand->setTimezone(timezone);
}
