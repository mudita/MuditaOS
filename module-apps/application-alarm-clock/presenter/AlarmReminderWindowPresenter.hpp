// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BasePresenter.hpp"
#include "application-alarm-clock/models/AlarmsRepository.hpp"
#include "application-alarm-clock/models/AlarmsReminderModel.hpp"

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
            ~Presenter() noexcept override = default;

            virtual void update(UserAction action)                                  = 0;
            virtual void startTimers(AlarmsReminderModel::OnTimerCallback callback) = 0;
            virtual void stopTimers()                                               = 0;
            virtual TimePoint getTimeToDisplay()                                    = 0;
            virtual bool setAlarmRecords(std::vector<AlarmsRecord> records)         = 0;
            virtual bool hasAlarmRecordFifthSnooze()                                = 0;
            virtual bool closeReminder()                                            = 0;
            virtual void updateAllAlarmRecords()                                    = 0;
            virtual void updateAudioToken(audio::Token token)                       = 0;
        };
    };

    class AlarmReminderWindowPresenter : public AlarmReminderWindowContract::Presenter
    {
      public:
        AlarmReminderWindowPresenter(std::unique_ptr<AbstractAlarmsRepository> &&alarmsRepository,
                                     std::unique_ptr<AbstractAlarmsReminderModel> &&alarmsReminderModel);

        void update(UserAction action) override;
        void startTimers(AlarmsReminderModel::OnTimerCallback callback) override;
        void stopTimers() override;
        TimePoint getTimeToDisplay() override;
        bool setAlarmRecords(std::vector<AlarmsRecord> records) override;
        bool hasAlarmRecordFifthSnooze() override;
        bool closeReminder() override;
        void updateAllAlarmRecords() override;
        void updateAudioToken(audio::Token token) override;

      private:
        void snoozeHandle(AlarmsRecord &alarm, uint32_t delay);
        void endAlarm(AlarmsRecord &alarm);
        void updatePreviousRecords(std::vector<AlarmsRecord> &records);
        std::unique_ptr<AbstractAlarmsRepository> alarmsRepository;
        std::unique_ptr<AbstractAlarmsReminderModel> alarmsReminderModel;
    };
} // namespace app::alarmClock
