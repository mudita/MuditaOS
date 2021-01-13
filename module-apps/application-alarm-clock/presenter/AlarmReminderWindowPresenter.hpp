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

            virtual void update(AlarmsRecord &alarm, UserAction action, uint32_t delay) = 0;
            virtual void updatePreviousRecords(std::vector<AlarmsRecord> &records)      = 0;
            virtual void startTimers(AlarmsReminderModel::OnTimerCallback callback)     = 0;
            virtual void stopTimers()                                                   = 0;
            virtual void handleMusicPlay(const std::string &filePath)                   = 0;
            virtual void stopMusic()                                                    = 0;
            virtual uint32_t getElapsedMinutes()                                        = 0;
            virtual TimePoint getTimeToDisplay(const AlarmsRecord &alarm)               = 0;
            virtual bool setAlarmRecords(std::vector<AlarmsRecord> records)             = 0;
            virtual AlarmsRecord &getAlarmRecord()                                      = 0;
            virtual std::vector<AlarmsRecord> getAllAlarmRecords()                      = 0;
            virtual void eraseFrontAlarmRecord()                                        = 0;
            virtual void updateAllAlarmRecords()                                        = 0;
        };
    };

    class AlarmReminderWindowPresenter : public AlarmReminderWindowContract::Presenter
    {
      public:
        AlarmReminderWindowPresenter(std::unique_ptr<AbstractAlarmsRepository> &&alarmsRepository,
                                     std::unique_ptr<AbstractAlarmsReminderModel> &&alarmsReminderModel);

        void update(AlarmsRecord &alarm, UserAction action, uint32_t delay) override;
        void updatePreviousRecords(std::vector<AlarmsRecord> &records) override;
        void startTimers(AlarmsReminderModel::OnTimerCallback callback) override;
        void stopTimers() override;
        void handleMusicPlay(const std::string &filePath) override;
        void stopMusic() override;
        uint32_t getElapsedMinutes() override;
        TimePoint getTimeToDisplay(const AlarmsRecord &alarm) override;
        bool setAlarmRecords(std::vector<AlarmsRecord> records) override;
        AlarmsRecord &getAlarmRecord() override;
        std::vector<AlarmsRecord> getAllAlarmRecords() override;
        void eraseFrontAlarmRecord() override;
        void updateAllAlarmRecords() override;

      private:
        void snoozeHandle(AlarmsRecord &alarm, uint32_t delay);
        void endAlarm(AlarmsRecord &alarm);
        std::unique_ptr<AbstractAlarmsRepository> alarmsRepository;
        std::unique_ptr<AbstractAlarmsReminderModel> alarmsReminderModel;
    };
} // namespace app::alarmClock
