// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>

using AlarmTimeStamp = std::chrono::time_point<std::chrono::system_clock>;

class AlarmObject
{
  public:
    AlarmObject() = default;
    explicit AlarmObject(AlarmTimeStamp alarmTimeStamp) : alarmTimeStamp(alarmTimeStamp)
    {}

    AlarmObject &operator=(const AlarmObject &other);

    bool operator!=(const AlarmObject &other) const;
    bool operator==(const AlarmObject &other) const;
    bool operator<(const AlarmObject &other) const;
    bool operator<=(const AlarmObject &other) const;
    bool operator>(const AlarmObject &other) const;
    bool operator>=(const AlarmObject &other) const;

    AlarmTimeStamp getTimeStamp() const;
    bool isYetToCome() const;
    bool isOutdated() const;

  private:
    AlarmTimeStamp alarmTimeStamp{};
};
