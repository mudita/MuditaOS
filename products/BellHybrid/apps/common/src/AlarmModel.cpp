// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/AlarmModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>
#include <time/dateCommon.hpp>
#include <service-time/AlarmServiceAPI.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/ServiceTimeName.hpp>
#include <service-time/AlarmMessage.hpp>

namespace app
{
    AlarmModel::AlarmModel(ApplicationCommon *app) : app::AsyncCallbackReceiver{app}, app{app}
    {
        state = State::InitInProgress;
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    void AlarmModel::update(AlarmModelReadyHandler callback)
    {
        responseCallback = [this, callback](const auto response) -> bool {
            const auto resp = dynamic_cast<alarms::AlarmGetWithStatusResponseMessage *>(response);
            if (resp) {
                if (resp->alarm.isValid()) {
                    auto alarm = resp->alarm;
                    updateCache(alarm.getNextSingleEvent(TimePointNow()), resp->status);
                    if (callback) {
                        callback();
                    }
                }
                else if (state == State::InitInProgress) {
                    /// We received invalid response from DB during model initialization. It means alarm record does not
                    /// exist, hence we need to create one.
                    auto defaultAlarm = generateDefaultAlarm();
                    auto request      = AsyncRequest::createFromMessage(
                        std::make_unique<alarms::AlarmAddRequestMessage>(defaultAlarm), service::name::service_time);
                    request->execute(this->app, this, responseCallback);

                    state = State::Valid;
                    this->update(callback);
                }
            }
            return true;
        };

        auto request = AsyncRequest::createFromMessage(std::make_unique<alarms::AlarmGetWithStatusRequestMessage>(),
                                                       service::name::service_time);
        request->execute(app, this, responseCallback);
    }
    void AlarmModel::setAlarmTime(time_t time)
    {
        auto alarmEventPtr = getAlarmPtr();
        if (!alarmEventPtr) {
            return;
        }

        if (alarmStatus == alarms::AlarmStatus::Snoozed) {
            disableSnooze(*alarmEventPtr);
            alarmStatus = alarms::AlarmStatus::Activated;
        }

        auto clockTime = std::localtime(&time);
        alarmEventPtr->alarmTime =
            AlarmTime{std::chrono::hours{clockTime->tm_hour}, std::chrono::minutes{clockTime->tm_min}};

        updateAlarm(*alarmEventPtr);
    }

    time_t AlarmModel::getAlarmTime() const
    {
        return Clock::to_time_t(cachedRecord.startDate);
    }
    void AlarmModel::activate(bool value)
    {
        auto alarmEventPtr = getAlarmPtr();
        if (!alarmEventPtr) {
            return;
        }
        alarmEventPtr->enabled = value;
        if (isSnoozeActive()) {
            disableSnooze(*alarmEventPtr);
        }
        updateAlarm(*alarmEventPtr);
        alarmStatus = value ? alarms::AlarmStatus::Activated : alarms::AlarmStatus::Deactivated;
    }
    void AlarmModel::updateAlarm(AlarmEventRecord &alarm)
    {
        auto request = AsyncRequest::createFromMessage(std::make_unique<alarms::AlarmUpdateRequestMessage>(alarm),
                                                       service::name::service_time);
        request->execute(app, this, [this](sys::ResponseMessage *) {
            update(nullptr);
            return true;
        });

        cachedRecord = alarm.getNextSingleEvent(TimePointNow());
    }
    void AlarmModel::disableSnooze(AlarmEventRecord &alarm)
    {
        auto request = AsyncRequest::createFromMessage(std::make_unique<alarms::TurnOffSnoozeRequestMessage>(alarm.ID),
                                                       service::name::service_time);
        request->execute(app, this, [this](sys::ResponseMessage *) {
            update(nullptr);
            return true;
        });
        nextSnoozeTime = TIME_POINT_INVALID;
    }
    bool AlarmModel::isActive() const
    {
        if (!cachedRecord.parent) {
            return false;
        }

        auto alarmEventPtr = getAlarmPtr();
        if (!alarmEventPtr) {
            return false;
        }
        return alarmEventPtr->enabled;
    }

    TimePoint AlarmModel::calculateNextSnoozeTime(std::uint32_t desiredSnoozeTime)
    {
        const auto now     = TimePointNow();
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        if ((seconds % 60) <= 15) {
            return std::chrono::floor<std::chrono::minutes>(now) + std::chrono::minutes(desiredSnoozeTime);
        }
        else {
            return std::chrono::ceil<std::chrono::minutes>(now) + std::chrono::minutes(desiredSnoozeTime);
        }
    }

    std::chrono::seconds AlarmModel::getSnoozeDuration() const
    {
        const auto snoozeDurationStr =
            settings.getValue(bell::settings::Snooze::length, settings::SettingsScope::Global);
        const auto snoozeDuration = utils::getNumericValue<std::uint32_t>(snoozeDurationStr);
        return std::chrono::minutes{snoozeDuration};
    }

    bool AlarmModel::isSnoozeAllowed()
    {
        const auto snoozeActiveStr = settings.getValue(bell::settings::Snooze::active, settings::SettingsScope::Global);
        const auto snoozeActive    = utils::getNumericValue<bool>(snoozeActiveStr);
        return snoozeActive;
    }
    void AlarmModel::turnOff()
    {
        snoozeCount    = 0;
        nextSnoozeTime = TIME_POINT_INVALID;
        if (cachedRecord.parent != nullptr) {
            alarms::AlarmServiceAPI::requestTurnOffRingingAlarm(app, cachedRecord.parent->ID);
        }
    }

    void AlarmModel::snooze()
    {
        snoozeCount++;
        const auto snoozeDurationStr =
            settings.getValue(bell::settings::Snooze::length, settings::SettingsScope::Global);
        const auto snoozeDuration = utils::getNumericValue<std::uint32_t>(snoozeDurationStr);

        nextSnoozeTime = calculateNextSnoozeTime(snoozeDuration);

        alarms::AlarmServiceAPI::requestSnoozeRingingAlarm(app, cachedRecord.parent->ID, nextSnoozeTime);
        alarmStatus = alarms::AlarmStatus::Snoozed;
    }

    std::chrono::seconds AlarmModel::getTimeToNextSnooze()
    {
        return std::chrono::duration_cast<std::chrono::seconds>(nextSnoozeTime - TimePointNow());
    }

    AlarmEventRecord AlarmModel::generateDefaultAlarm() const
    {
        auto defaultAlarm      = AlarmEventRecord{};
        defaultAlarm.alarmTime = AlarmTime{7h, 0min};
        defaultAlarm.rruleText = "FREQ=DAILY";
        defaultAlarm.enabled   = true;
        return defaultAlarm;
    }

    std::shared_ptr<AlarmEventRecord> AlarmModel::getAlarmPtr() const
    {
        if (!cachedRecord.parent) {
            LOG_ERROR("Cached event has no parent");
            return nullptr;
        }
        auto alarmEventPtr = std::dynamic_pointer_cast<AlarmEventRecord>(cachedRecord.parent);
        if (!alarmEventPtr) {
            LOG_ERROR("Processed event is not an alarm");
            return nullptr;
        }
        return alarmEventPtr;
    }
    bool AlarmModel::isSnoozeActive()
    {
        return snoozeCount > 0;
    }
    void AlarmModel::updateCache(const SingleEventRecord &record, alarms::AlarmStatus status)
    {
        if (record.startDate != cachedRecord.startDate) {
            snoozeCount = 0;
        }
        cachedRecord = record;
        alarmStatus  = status;
        state        = State::Valid;
    }
    void AlarmModel::setDefaultAlarmTime()
    {
        auto alarmEventPtr = getAlarmPtr();
        if (!alarmEventPtr) {
            return;
        }
        alarmEventPtr->alarmTime = AlarmTime{7h, 00min};

        updateAlarm(*alarmEventPtr);
    }

    void AlarmModel::activateAlarm(bool state)
    {
        auto callback = [this, state]() { activate(state); };
        update(callback);
    }

    alarms::AlarmStatus AlarmModel::getAlarmStatus()
    {
        return alarmStatus;
    }
    TimePoint AlarmModel::getTimeOfNextSnooze()
    {
        return nextSnoozeTime;
    }
} // namespace app
