// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include <module-db/Interface/AlarmsRecord.hpp>
#include <module-sys/Service/Timer.hpp>

namespace app::alarmClock
{
    class AbstractAlarmsReminderModel
    {
      public:
        using OnTimerCallback = std::function<void()>;

        virtual ~AbstractAlarmsReminderModel() noexcept = default;

        virtual void startTimers(OnTimerCallback callback)        = 0;
        virtual void stopTimers()                                 = 0;
        virtual void handleMusicPlay(const std::string &filePath) = 0;
        virtual void stopMusic()                                  = 0;
        virtual uint32_t getElapsedMinutes()                      = 0;
        virtual uint32_t getPreviousElapsedMinutes()              = 0;
        virtual void resetPreviousElapsedSeconds()                = 0;
        virtual AlarmsRecord &getAlarmRecord()                                                                = 0;
        virtual std::vector<AlarmsRecord> getAllAlarmRecords() const                                          = 0;
        virtual void eraseFrontAlarmRecord()                                                                  = 0;
        virtual void clearAlarmRecords()                                                                      = 0;
        virtual std::pair<bool, std::vector<AlarmsRecord>> setAlarmRecords(std::vector<AlarmsRecord> records) = 0;
    };

    class AlarmsReminderModel : public AbstractAlarmsReminderModel
    {
      public:
        explicit AlarmsReminderModel(Application *app);

        void startTimers(OnTimerCallback callback) override;
        void stopTimers() override;
        void handleMusicPlay(const std::string &filePath) override;
        void stopMusic() override;
        uint32_t getElapsedMinutes() override;
        uint32_t getPreviousElapsedMinutes() override;
        void resetPreviousElapsedSeconds() override;
        AlarmsRecord &getAlarmRecord() override;
        std::vector<AlarmsRecord> getAllAlarmRecords() const override;
        void eraseFrontAlarmRecord() override;
        void clearAlarmRecords() override;
        std::pair<bool, std::vector<AlarmsRecord>> setAlarmRecords(std::vector<AlarmsRecord> records) override;

      private:
        void countElapsedSeconds();
        void startMusicTimer(uint32_t duration, const std::string &path);
        void playMusic(const std::string &path);
        Application *app = nullptr;
        std::unique_ptr<sys::Timer> reminderTimer;
        std::unique_ptr<sys::Timer> musicTimer;
        std::unique_ptr<sys::Timer> delayTimer;
        std::vector<AlarmsRecord> alarmRecords;
        std::vector<AlarmsRecord> previousAlarmRecords;

        uint32_t elapsedSeconds         = 0;
        uint32_t previousElapsedSeconds = 0;
    };
} // namespace app::alarmClock
