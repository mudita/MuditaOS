// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

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

        virtual bool isActive() const             = 0;
        virtual void setDefaultAlarmTime()        = 0;
        virtual void setAlarmTime(time_t time)    = 0;
        virtual time_t getAlarmTime() const       = 0;
        virtual void activate(bool value)         = 0;
        virtual std::uint32_t getSnoozeDuration() = 0;
        virtual bool isSnoozeAllowed()            = 0;
        virtual bool isSnoozeActive()             = 0;
        virtual void turnOff()                    = 0;
        virtual void snooze()                     = 0;
        virtual std::chrono::seconds getTimeToNextSnooze() = 0;
        /// Command model to update its internal data
        virtual void update(AlarmModelReadyHandler callback = AlarmModelReadyHandler()) = 0;
    };

} // namespace app
