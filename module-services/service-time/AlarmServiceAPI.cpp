// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Constants.hpp"

#include <service-time/AlarmMessage.hpp>
#include <service-time/AlarmServiceAPI.hpp>

#include <module-db/Interface/AlarmEventRecord.hpp>

namespace alarms
{

    namespace AlarmServiceAPI
    {
        template <class requestType, typename... Types>
        bool sendRequest(sys::Service *serv, Types &&...messageArgs)
        {
            auto msg = std::make_shared<requestType>(std::forward<Types>(messageArgs)...);
            return serv->bus.sendUnicast(msg, service::name::service_time);
        }

        bool requestGetAlarm(sys::Service *serv, unsigned int id)
        {
            return sendRequest<AlarmGetRequestMessage>(serv, id);
        }

        bool requestAddAlarm(sys::Service *serv, const AlarmEventRecord &alarmEvent)
        {
            return sendRequest<AlarmAddRequestMessage>(serv, alarmEvent);
        }

        bool requestUpdateAlarm(sys::Service *serv, const AlarmEventRecord &alarmEvent)
        {
            return sendRequest<AlarmUpdateRequestMessage>(serv, alarmEvent);
        }

        bool requestRemoveAlarm(sys::Service *serv, unsigned int id)
        {
            return sendRequest<AlarmRemoveRequestMessage>(serv, id);
        }

        bool requestGetAlarmsInRange(sys::Service *serv, unsigned int offset, unsigned int limit)
        {
            return sendRequest<AlarmsGetInRangeRequestMessage>(serv, offset, limit);
        }

        bool requestGetNextSingleEvents(sys::Service *serv)
        {
            return sendRequest<AlarmGetNextSingleEventsRequestMessage>(serv);
        }

        bool requestTurnOffRingingAlarm(sys::Service *serv, const std::uint32_t id)
        {
            return sendRequest<RingingAlarmTurnOffRequestMessage>(serv, id);
        }

        bool requestSnoozeRingingAlarm(sys::Service *serv, const std::uint32_t id, const TimePoint nextAlarmTime)
        {
            return sendRequest<RingingAlarmSnoozeRequestMessage>(serv, id, nextAlarmTime);
        }

        bool requestStopAllSnoozedAlarms(sys::Service *serv)
        {
            return sendRequest<StopAllSnoozedAlarmsRequestMessage>(serv);
        }

        bool requestRegisterSnoozedAlarmsCountChangeCallback(sys::Service *serv)
        {
            return sendRequest<RegisterSnoozedAlarmsCountChangeHandlerRequestMessage>(serv);
        }

        bool requestRegisterActiveAlarmsIndicatorHandler(sys::Service *serv)
        {
            return sendRequest<RegisterActiveAlarmsIndicatorHandlerRequestMessage>(serv);
        }
    }; // namespace AlarmServiceAPI

} // namespace alarms
