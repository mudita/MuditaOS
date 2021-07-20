// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmRepository.hpp"

BellAlarmRepository::BellAlarmRepository()
{}

unsigned int BellAlarmRepository::getSystemHour()
{
    return alarmHour;
}

unsigned int BellAlarmRepository::getSystemMinute()
{
    return alarmMinute;
}

BellAlarm::Status BellAlarmRepository::getSystemAlarmStatus()
{
    return alarmStatus;
}

utils::time::Locale::TimeFormat BellAlarmRepository::getSystemTimeFormat()
{
    return alarmTimeFormat;
}
