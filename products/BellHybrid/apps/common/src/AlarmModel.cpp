// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/AlarmModel.hpp"

#include <apps-common/Application.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <time/dateCommon.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/Constants.hpp>
#include <service-time/AlarmMessage.hpp>

namespace app
{
    AlarmModel::AlarmModel(Application *app) : app::AsyncCallbackReceiver{app}, app{app}
    {
        responseCallback = [this](const auto response) -> bool {
            const auto resp = dynamic_cast<alarms::AlarmGetResponseMessage *>(response);
            if (resp) {
                cachedRecord = resp->alarm;
            }
            return true;
        };

        requestAlarm();
    }
    void AlarmModel::requestAlarm()
    {
        auto request = AsyncRequest::createFromMessage(std::make_unique<alarms::AlarmGetRequestMessage>(),
                                                       service::name::service_time);
        request->execute(app, this, responseCallback);
    }
    void AlarmModel::setAlarmTime(time_t time)
    {
        cachedRecord.startDate = Clock::from_time_t(time);

        updateAlarm(cachedRecord);
    }
    time_t AlarmModel::getAlarmTime() const
    {
        return Clock::to_time_t(cachedRecord.startDate);
    }
    void AlarmModel::activate(bool value)
    {
        cachedRecord.enabled = value;
        updateAlarm(cachedRecord);
    }
    void AlarmModel::updateAlarm(const AlarmEventRecord &alarm)
    {
        auto request = AsyncRequest::createFromMessage(std::make_unique<alarms::AlarmUpdateRequestMessage>(alarm),
                                                       service::name::service_time);
        request->execute(app, this, responseCallback);

        requestAlarm();
    }
    bool AlarmModel::isActive() const
    {
        return cachedRecord.enabled;
    }
} // namespace app
