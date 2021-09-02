// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmsRepository.hpp"
#include <module-db/queries/alarms/QueryAlarmsGetLimited.hpp>
#include <module-db/queries/alarms/QueryAlarmsAdd.hpp>
#include <module-db/queries/alarms/QueryAlarmsRemove.hpp>
#include <module-db/queries/alarms/QueryAlarmsEdit.hpp>
#include <module-db/queries/alarms/QueryAlarmsTurnOffAll.hpp>
#include <service-time/include/service-time/AlarmMessage.hpp>
#include <service-time/Constants.hpp>

#include "AsyncTask.hpp"
#include "log.hpp"

namespace app::alarmClock
{
    auto async = app::AsyncRequest::createFromMessage;

    AlarmsDBRepository::AlarmsDBRepository(Application *application)
        : AsyncCallbackReceiver{application}, application{application}
    {}

    void AlarmsDBRepository::getLimited(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback)
    {
        auto request =
            std::make_unique<alarms::AlarmsGetInRangeRequestMessage>(TIME_POINT_INVALID, TIME_POINT_MAX, offset, limit);
        auto task = async(std::move(request), service::name::service_time); // -> std::unique_ptr<AsyncRequest>;
        auto cb   = [callback](auto response) {
            auto result = dynamic_cast<alarms::AlarmsGetInRangeResponseMessage *>(response);
            if (result == nullptr) {
                LOG_ERROR("BAD RESULT");
                return false;
            }
            if (callback) {
                callback(result->alarms, result->alarms.size());
            }
            return true;
        };
        task->execute(application, this, cb);
    }

    template <class Request, class Result, typename... Args>
    void AlarmsDBRepository::GetQuery(const AbstractAlarmsRepository::OnResultCallback &callback, Args... args)
    {
        auto task = async(std::make_unique<Request>(args...), service::name::service_time);
        auto cb   = [callback](auto response) {
            auto result = dynamic_cast<Result *>(response);
            if (result == nullptr) {
                LOG_ERROR("BAD RESULT");
                return false;
            }
            if (callback) {
                callback(result->success);
            }
            return true;
        };
        task->execute(application, this, cb);
    }

    void AlarmsDBRepository::add(const AlarmEventRecord &alarm,
                                 const AbstractAlarmsRepository::OnResultCallback &callback)
    {
        GetQuery<alarms::AlarmAddRequestMessage, alarms::AlarmAddResponseMessage>(callback, alarm);
    }

    void AlarmsDBRepository::remove(const AlarmEventRecord &alarm,
                                    const AbstractAlarmsRepository::OnResultCallback &callback)
    {
        GetQuery<alarms::AlarmRemoveRequestMessage, alarms::AlarmRemoveResponseMessage>(callback, alarm.ID);
    }

    void AlarmsDBRepository::update(const AlarmEventRecord &alarm,
                                    const AbstractAlarmsRepository::OnResultCallback &callback)
    {
        GetQuery<alarms::AlarmUpdateRequestMessage, alarms::AlarmUpdateResponseMessage>(callback, alarm);
    }

    void AlarmsDBRepository::turnOffAll(const AbstractAlarmsRepository::OnResultCallback &callback)
    {
    }
} // namespace app::alarmClock
