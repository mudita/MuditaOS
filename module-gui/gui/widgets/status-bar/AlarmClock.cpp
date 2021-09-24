// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmClock.hpp"
#include "Style.hpp"

namespace gui::status_bar
{
    constexpr auto alarm_status = "alarmclock_status";

    AlarmClock::AlarmClock(Item *parent, uint32_t x, uint32_t y) : StatusBarWidgetBase(parent, x, y, 0, 0)
    {
        set(alarm_status, style::status_bar::imageTypeSpecifier);
    }

    void AlarmClock::setAlarmClockStatus(bool status)
    {
        setVisible(status);
    }
}; // namespace gui::status_bar
