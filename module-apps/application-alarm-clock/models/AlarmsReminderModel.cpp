// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmsReminderModel.hpp"
#include "application-alarm-clock/ApplicationAlarmClock.hpp"
#include <module-services/service-audio/service-audio/AudioServiceAPI.hpp>

namespace app::alarmClock
{
    constexpr static const int reminderLifeDuration = 900000; // 15min
    constexpr static const int defaultDuration      = 10000;  // 10s

    AlarmsReminderModel::AlarmsReminderModel(Application *app)
        : app{app}, reminderTimer{std::make_unique<sys::Timer>(
                        "AlarmClockReminderTimer", app, reminderLifeDuration, sys::Timer::Type::SingleShot)},
          musicTimer{
              std::make_unique<sys::Timer>("AlarmClockMusicTimer", app, defaultDuration, sys::Timer::Type::Periodic)},
          delayTimer{std::make_unique<sys::Timer>("AlarmDelayTimer", app, defaultDuration, sys::Timer::Type::Periodic)}
    {}

    void AlarmsReminderModel::startTimers(OnTimerCallback callback)
    {
        elapsedSeconds = 0;
        reminderTimer->connect([=](sys::Timer &) { callback(); });
        reminderTimer->reload();
        delayTimer->connect([=](sys::Timer &) { countElapsedSeconds(); });
        delayTimer->reload();
    }

    void AlarmsReminderModel::startMusicTimer(uint32_t duration, const std::string &path)
    {
        musicTimer->setInterval(duration);
        musicTimer->connect([=](sys::Timer &) { playMusic(path); });
        musicTimer->reload();
    }

    void AlarmsReminderModel::stopTimers()
    {
        reminderTimer->stop();
        delayTimer->stop();
        musicTimer->stop();
    }

    void AlarmsReminderModel::handleMusicPlay(const std::string &filePath)
    {
        auto fileTags = AudioServiceAPI::GetFileTags(app, filePath);
        if (fileTags != std::nullopt) {
            startMusicTimer(fileTags->duration_sec * utils::time::milisecondsInSecond, fileTags->filePath);
            playMusic(fileTags->filePath);
        }
    }

    void AlarmsReminderModel::playMusic(const std::string &path)
    {
        AudioServiceAPI::PlaybackStart(app, audio::PlaybackType::Multimedia, path);
    }

    void AlarmsReminderModel::stopMusic()
    {
        auto alarmApp = static_cast<ApplicationAlarmClock *>(app);
        AudioServiceAPI::Stop(app, alarmApp->getAudioToken());
    }

    void AlarmsReminderModel::countElapsedSeconds()
    {
        elapsedSeconds += defaultDuration / utils::time::milisecondsInSecond;
        previousElapsedSeconds = elapsedSeconds;
    }

    uint32_t AlarmsReminderModel::getElapsedMinutes()
    {
        return std::lround(static_cast<float>(elapsedSeconds) / utils::time::secondsInMinute);
    }

    uint32_t AlarmsReminderModel::getPreviousElapsedMinutes()
    {
        return std::lround(static_cast<float>(previousElapsedSeconds) / utils::time::secondsInMinute);
    }

    void AlarmsReminderModel::resetPreviousElapsedSeconds()
    {
        previousElapsedSeconds = 0;
    }

    std::pair<bool, std::vector<AlarmsRecord>> AlarmsReminderModel::setAlarmRecords(std::vector<AlarmsRecord> records)
    {
        std::vector<AlarmsRecord> forUpdate;
        alarmRecords = records;
        if (!previousAlarmRecords.empty()) {
            if (previousAlarmRecords.front().ID == alarmRecords.front().ID) {
                LOG_DEBUG("Duplicated data received, return");
                return std::make_pair(false, forUpdate);
            }
            LOG_DEBUG("New alarm covered the old one, automatic snooze applying");
            forUpdate = previousAlarmRecords;
        }
        previousAlarmRecords = alarmRecords;
        return std::make_pair(true, forUpdate);
    }

    AlarmsRecord &AlarmsReminderModel::getAlarmRecord()
    {
        return alarmRecords.front();
    }

    std::vector<AlarmsRecord> AlarmsReminderModel::getAllAlarmRecords() const
    {
        return alarmRecords;
    }

    void AlarmsReminderModel::eraseFrontAlarmRecord()
    {
        if (!alarmRecords.empty()) {
            alarmRecords.erase(alarmRecords.begin());
        }
    }

    void AlarmsReminderModel::clearAlarmRecords()
    {
        alarmRecords.clear();
    }
} // namespace app::alarmClock
