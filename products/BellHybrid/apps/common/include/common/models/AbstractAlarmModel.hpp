// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ctime>

namespace app
{
    class AbstractAlarmModel
    {
      public:
        virtual ~AbstractAlarmModel() noexcept = default;

        virtual bool isActive() const          = 0;
        virtual void setAlarmTime(time_t time) = 0;
        virtual time_t getAlarmTime() const    = 0;
        virtual void activate(bool value)      = 0;
    };

} // namespace app
