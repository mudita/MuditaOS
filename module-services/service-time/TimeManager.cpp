// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-time/TimeManager.hpp>
#include <service-time/TimezoneHandler.hpp>

#include <ctime>

void TimeManager::handleCellularTimeUpdate(struct tm time, const std::string &timezone)
{
    rtcCommand->setTime(time);
    rtcCommand->setTimezone(timezone);
}

void TimeManager::handleTimeChangeRequest(const time_t &time)
{
    rtcCommand->setTime(time);
}

void TimeManager::handleTimezoneChangeRequest(const std::string &timezoneRules)
{
    rtcCommand->setTimezone(timezoneRules);
}
