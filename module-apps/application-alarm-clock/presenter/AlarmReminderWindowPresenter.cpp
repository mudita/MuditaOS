// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmReminderWindowPresenter.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"

namespace app::alarmClock
{
    AlarmReminderWindowPresenter::AlarmReminderWindowPresenter(
        std::unique_ptr<AbstractAlarmsRepository> &&alarmsRepository)
        : alarmsRepository{std::move(alarmsRepository)}
    {}

    void AlarmReminderWindowPresenter::update(AlarmsRecord &alarm, UserAction action, uint32_t delay)
    {
        if (action == UserAction::Snooze) {
            snoozeHandle(alarm, delay);
        }
        else {
            endAlarm(alarm);
        }
        alarmsRepository->update(alarm, nullptr);
    }

    void AlarmReminderWindowPresenter::snoozeHandle(AlarmsRecord &alarm, uint32_t delay)
    {
        alarm.delay += delay;
        switch (alarm.status) {
        case AlarmStatus::Off:
            break;
        case AlarmStatus::On:
            alarm.status = AlarmStatus::FirstSnooze;
            break;
        case AlarmStatus::FirstSnooze:
            alarm.status = AlarmStatus::SecondSnooze;
            break;
        case AlarmStatus::SecondSnooze:
            alarm.status = AlarmStatus::ThirdSnooze;
            break;
        case AlarmStatus::ThirdSnooze:
            alarm.status = AlarmStatus::FourthSnooze;
            break;
        case AlarmStatus::FourthSnooze:
            alarm.status = AlarmStatus::FifthSnooze;
            break;
        case AlarmStatus::FifthSnooze:
            endAlarm(alarm);
            break;
        }
    }

    void AlarmReminderWindowPresenter::endAlarm(AlarmsRecord &alarm)
    {
        if (alarm.repeat == static_cast<uint32_t>(AlarmRepeat::never)) {
            alarm.status = AlarmStatus::Off;
        }
        else {
            alarm.status = AlarmStatus::On;
        }
        alarm.delay = 0;
    }
} // namespace app::alarmClock
