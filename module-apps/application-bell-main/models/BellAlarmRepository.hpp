// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-bell-main/data/Bell_alarm_data.hpp"
#include "time/time_locale.hpp"

#include <utility>
#include <memory>

class AbstractBellAlarmRepository
{
  public:
    virtual ~AbstractBellAlarmRepository() noexcept = default;

    virtual unsigned int getSystemHour()                          = 0;
    virtual unsigned int getSystemMinute()                        = 0;
    virtual BellAlarm::Status getSystemAlarmStatus()              = 0;
    virtual utils::time::Locale::TimeFormat getSystemTimeFormat() = 0;
};

class BellAlarmRepository : public AbstractBellAlarmRepository
{
  public:
    BellAlarmRepository();

    unsigned int getSystemHour() override;
    unsigned int getSystemMinute() override;
    BellAlarm::Status getSystemAlarmStatus() override;
    utils::time::Locale::TimeFormat getSystemTimeFormat() override;

  private:
    utils::time::Locale::TimeFormat alarmTimeFormat = utils::time::Locale::defaultTimeFormat;
    BellAlarm::Status alarmStatus                   = BellAlarm::Status::DEACTIVATED;
    unsigned int alarmHour                          = 10;
    unsigned int alarmMinute                        = 20;
};
