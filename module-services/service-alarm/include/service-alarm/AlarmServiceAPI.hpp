#pragma once

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/Service/Service.hpp>

#include <ctime>
#include <vector>

class AlarmEventRecord;

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
        bool requestGetAlarmsInRange(sys::Service *serv, TimePoint start, TimePoint end);
        bool requestGetNextSingleEvents(sys::Service *serv);
    }; // namespace AlarmServiceAPI

} // namespace alarms
