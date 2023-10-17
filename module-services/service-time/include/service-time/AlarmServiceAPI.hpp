// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>

#include <time/dateCommon.hpp>

#include <ctime>
#include <vector>

struct AlarmEventRecord;

/**
 * @brief Basic interface alarm API
 */
namespace alarms
{

    namespace AlarmServiceAPI
    {
        bool requestGetAlarm(sys::Service *serv, unsigned int id);
        bool requestAddAlarm(sys::Service *serv, const AlarmEventRecord &alarmEvent);
        bool requestUpdateAlarm(sys::Service *serv, const AlarmEventRecord &alarmEvent);
        bool requestRemoveAlarm(sys::Service *serv, unsigned int id);
        bool requestGetAlarmsInRange(sys::Service *serv, unsigned int offset, unsigned int limit);
        bool requestGetNextSingleEvents(sys::Service *serv);
        bool requestTurnOffRingingAlarm(sys::Service *serv, const std::uint32_t id);
        bool requestSnoozeRingingAlarm(sys::Service *serv, const std::uint32_t id, const TimePoint nextAlarmTime);
        bool requestStopAllSnoozedAlarms(sys::Service *serv);
        bool requestRegisterSnoozedAlarmsCountChangeCallback(sys::Service *serv);
        bool requestRegisterActiveAlarmsIndicatorHandler(sys::Service *serv);
    }; // namespace AlarmServiceAPI

} // namespace alarms
