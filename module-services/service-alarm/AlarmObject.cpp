// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmObject.hpp"

AlarmObject &AlarmObject::operator=(const AlarmObject &other)
{
    if (this != &other) {
        alarmTimeStamp = other.getTimeStamp();
    }
    return *this;
}

bool AlarmObject::operator!=(const AlarmObject &other) const
{
    return (alarmTimeStamp != other.getTimeStamp());
}

bool AlarmObject::operator==(const AlarmObject &other) const
{
    return (alarmTimeStamp == other.getTimeStamp());
}

bool AlarmObject::operator<(const AlarmObject &other) const
{
    return (alarmTimeStamp < other.getTimeStamp());
}

bool AlarmObject::operator<=(const AlarmObject &other) const
{
    return (alarmTimeStamp <= other.getTimeStamp());
}

bool AlarmObject::operator>(const AlarmObject &other) const
{
    return (alarmTimeStamp > other.getTimeStamp());
}

bool AlarmObject::operator>=(const AlarmObject &other) const
{
    return (alarmTimeStamp >= other.getTimeStamp());
}

AlarmTimeStamp AlarmObject::getTimeStamp() const
{
    return alarmTimeStamp;
}

bool AlarmObject::isYetToCome() const
{
    return (alarmTimeStamp > std::chrono::system_clock::now());
}

bool AlarmObject::isOutdated() const
{
    return (alarmTimeStamp <= std::chrono::system_clock::now());
}
