// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmReminderWindowPresenter.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"

namespace app::alarmClock
{
    AlarmReminderWindowPresenter::AlarmReminderWindowPresenter(
        std::unique_ptr<AbstractAlarmsRepository> &&alarmsRepository,
        std::unique_ptr<AbstractAlarmsReminderModel> &&alarmsReminderModel)
        : alarmsRepository{std::move(alarmsRepository)}, alarmsReminderModel{std::move(alarmsReminderModel)}
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

    void AlarmReminderWindowPresenter::updatePreviousRecords(std::vector<AlarmsRecord> &records)
    {
        for (auto &alarm : records) {
            update(alarm, UserAction::Snooze, alarmsReminderModel->getPreviousElapsedMinutes());
        }
        alarmsReminderModel->resetPreviousElapsedSeconds();
    }

    void AlarmReminderWindowPresenter::startTimers(AlarmsReminderModel::OnTimerCallback callback)
    {
        alarmsReminderModel->startTimers(callback);
    }

    void AlarmReminderWindowPresenter::stopTimers()
    {
        alarmsReminderModel->stopTimers();
    }

    void AlarmReminderWindowPresenter::handleMusicPlay(const std::string &filePath)
    {
        alarmsReminderModel->handleMusicPlay(filePath);
    }

    void AlarmReminderWindowPresenter::stopMusic()
    {
        alarmsReminderModel->stopMusic();
    }

    uint32_t AlarmReminderWindowPresenter::getElapsedMinutes()
    {
        return alarmsReminderModel->getElapsedMinutes();
    }

    TimePoint AlarmReminderWindowPresenter::getTimeToDisplay(const AlarmsRecord &alarm)
    {
        auto timeToDisplay = alarm.time + std::chrono::minutes(alarm.delay);
        if (alarm.status > AlarmStatus::On) {
            timeToDisplay += (static_cast<uint32_t>(alarm.status) - 1) * std::chrono::minutes(alarm.snooze);
        }
        return timeToDisplay;
    }

    bool AlarmReminderWindowPresenter::setAlarmRecords(std::vector<AlarmsRecord> records)
    {
        auto [ret, recordsForUpdate] = alarmsReminderModel->setAlarmRecords(records);
        if (!recordsForUpdate.empty()) {
            updatePreviousRecords(recordsForUpdate);
        }
        return ret;
    }

    AlarmsRecord &AlarmReminderWindowPresenter::getAlarmRecord()
    {
        return alarmsReminderModel->getAlarmRecord();
    }

    std::vector<AlarmsRecord> AlarmReminderWindowPresenter::getAllAlarmRecords()
    {
        return alarmsReminderModel->getAllAlarmRecords();
    }

    void AlarmReminderWindowPresenter::eraseFrontAlarmRecord()
    {
        alarmsReminderModel->eraseFrontAlarmRecord();
    }

    void AlarmReminderWindowPresenter::updateAllAlarmRecords()
    {
        for (auto &alarm : alarmsReminderModel->getAllAlarmRecords()) {
            update(alarm, UserAction::Snooze, getElapsedMinutes());
        }
        alarmsReminderModel->clearAlarmRecords();
    }
} // namespace app::alarmClock
