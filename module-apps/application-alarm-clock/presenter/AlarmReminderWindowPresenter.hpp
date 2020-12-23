// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BasePresenter.hpp"
#include "application-alarm-clock/models/AlarmsRepository.hpp"

namespace app::alarmClock
{
    enum class UserAction
    {
        TurnOff,
        Snooze
    };

    class AlarmReminderWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<AlarmReminderWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            virtual void update(AlarmsRecord &alarm, UserAction action, uint32_t delay) = 0;
        };
    };

    class AlarmReminderWindowPresenter : public AlarmReminderWindowContract::Presenter
    {
      public:
        explicit AlarmReminderWindowPresenter(std::unique_ptr<AbstractAlarmsRepository> &&alarmsRepository);

        void update(AlarmsRecord &alarm, UserAction action, uint32_t delay) override;

      private:
        void snoozeHandle(AlarmsRecord &alarm, uint32_t delay);
        void endAlarm(AlarmsRecord &alarm);
        std::unique_ptr<AbstractAlarmsRepository> alarmsRepository;
    };
} // namespace app::alarmClock
