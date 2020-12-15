// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmReminderWindowPresenter.hpp"

namespace app::alarmClock
{
    AlarmReminderWindowPresenter::AlarmReminderWindowPresenter(
        std::unique_ptr<AbstractAlarmsRepository> &&alarmsRepository)
        : alarmsRepository{std::move(alarmsRepository)}
    {}

    void AlarmReminderWindowPresenter::update(const AlarmsRecord &alarm)
    {
        alarmsRepository->update(alarm, nullptr);
    }
} // namespace app::alarmClock
