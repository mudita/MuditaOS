#pragma once

// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmMessage.hpp"
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
        bool getAlarm(sys::Service *serv, unsigned int id);
        bool addAlarm(sys::Service *serv, const AlarmEventRecord &alarmEvent);
        bool updateAlarm(sys::Service *serv, const AlarmEventRecord &alarmEvent);
        bool removeAlarm(sys::Service *serv, unsigned int id);
        bool getAlarmsInRange(sys::Service *serv, TimePoint start, TimePoint end);
        bool getNextSingleEvents(sys::Service *serv);
    }; // namespace AlarmServiceAPI

} // namespace alarms
