// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/AlarmModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <time/dateCommon.hpp>
#include <service-time/AlarmServiceAPI.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/Constants.hpp>
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
            const auto resp = dynamic_cast<alarms::AlarmGetResponseMessage *>(response);
            if (resp) {
                if (resp->alarm.isValid()) {
                    auto alarm   = resp->alarm;
                    cachedRecord = alarm.getNextSingleEvent(TimePointNow());
                    state        = State::Valid;
                    snoozeCount  = 0;
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

        auto request = AsyncRequest::createFromMessage(std::make_unique<alarms::AlarmGetRequestMessage>(),
                                                       service::name::service_time);
        request->execute(app, this, responseCallback);
    }
    void AlarmModel::setAlarmTime(time_t time)
    {
        auto alarmEventPtr = getAlarmPtr();
        if (!alarmEventPtr) {
            return;
        }
        alarmEventPtr->startDate = Clock::from_time_t(time);

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
        updateAlarm(*alarmEventPtr);
    }
    void AlarmModel::updateAlarm(AlarmEventRecord &alarm)
    {
        auto request = AsyncRequest::createFromMessage(std::make_unique<alarms::AlarmUpdateRequestMessage>(alarm),
                                                       service::name::service_time);
        request->execute(app, this, responseCallback);

        cachedRecord = alarm.getNextSingleEvent(TimePointNow());
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
    std::uint32_t AlarmModel::getSnoozeDuration()
    {
        const auto snoozeDurationStr =
            settings.getValue(bell::settings::Snooze::length, settings::SettingsScope::Global);
        const auto snoozeDuration = utils::getNumericValue<std::uint32_t>(snoozeDurationStr);

        return snoozeDuration;
    }
    bool AlarmModel::isSnoozeAllowed()
    {
        const auto snoozeActiveStr = settings.getValue(bell::settings::Snooze::active, settings::SettingsScope::Global);
        const auto snoozeActive    = utils::getNumericValue<bool>(snoozeActiveStr);

        return (snoozeActive && snoozeCount < maxSnoozeCount);
    }
    void AlarmModel::turnOff()
    {
        snoozeCount = 0;
        alarms::AlarmServiceAPI::requestTurnOffRingingAlarm(app, cachedRecord.parent->ID);
    }

    void AlarmModel::snooze()
    {
        const auto snoozeDurationStr =
            settings.getValue(bell::settings::Snooze::length, settings::SettingsScope::Global);
        const auto snoozeDuration = utils::getNumericValue<std::uint32_t>(snoozeDurationStr);

        snoozeCount++;
        auto newAlarmTime =
            std::chrono::floor<std::chrono::minutes>(TimePointNow()) + std::chrono::minutes(snoozeDuration);
        alarms::AlarmServiceAPI::requestSnoozeRingingAlarm(app, cachedRecord.parent->ID, newAlarmTime);
    }

    AlarmEventRecord AlarmModel::generateDefaultAlarm() const
    {
        auto defaultAlarm      = AlarmEventRecord{};
        defaultAlarm.startDate = TimePointFromString("2021-01-01 07:00:00");
        defaultAlarm.rruleText = "FREQ=DAILY";
        defaultAlarm.endDate   = TIME_POINT_MAX;
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
} // namespace app
