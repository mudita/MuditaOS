// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-time/AlarmStatus.hpp>
#include <time/dateCommon.hpp>

#include <chrono>
#include <ctime>
#include <cstdint>
#include <functional>

namespace app
{
    using AlarmModelReadyHandler = std::function<void()>;

    class AbstractAlarmModel
    {
      public:
        virtual ~AbstractAlarmModel() noexcept = default;

        virtual bool isActive() const                          = 0;
        virtual void setDefaultAlarmTime()                     = 0;
        virtual void setAlarmTime(time_t time)                 = 0;
        virtual time_t getAlarmTime() const                    = 0;
        virtual void activate(bool value)                      = 0;
        virtual std::chrono::seconds getSnoozeDuration() const = 0;
        virtual bool isSnoozeAllowed()                         = 0;
        virtual bool isSnoozeActive()                          = 0;
        virtual void turnOff()                                 = 0;
        virtual void snooze()                                  = 0;
        virtual std::chrono::seconds getTimeToNextSnooze()     = 0;
        virtual TimePoint getTimeOfNextSnooze()                = 0;
        virtual alarms::AlarmStatus getAlarmStatus()           = 0;
        virtual void activateAlarm(bool state)                 = 0;
        /// Command model to update its internal data
        virtual void update(AlarmModelReadyHandler callback = AlarmModelReadyHandler()) = 0;
    };

} // namespace app
