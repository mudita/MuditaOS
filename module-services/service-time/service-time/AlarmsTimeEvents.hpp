// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "TimeEvents.hpp"

#include <application-calendar/data/dateCommon.hpp>
#include <module-db/Interface/AlarmsRecord.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace db
{
    class QueryResult;
} // namespace db
namespace sys
{
    class Service;
} // namespace sys

namespace stm
{
    class AlarmsTimeEvents : public TimeEvents
    {
      private:
        bool isInvalid = false;
        std::vector<AlarmsRecord> alarmRecords;

      protected:
        const std::string timerName() override
        {
            return "AlarmsTimeEvents_timer";
        };
        uint32_t calcToNextEventInterval(std::unique_ptr<db::QueryResult> nextEventQueryResult) override;

        bool sendNextEventQuery() override;
        bool sendEventFiredQuery() override;
        void invokeEvent() override;
        bool isAlarmGoingToJumpToNextDay(AlarmsRecord record);
        std::vector<AlarmsRecord> customRepeatHandle(const AlarmsRecord &record, uint32_t weekDay);
        void applySnoozeAndDelay(std::vector<AlarmsRecord> &records);
        void sortNearestAlarms(std::vector<AlarmsRecord> &nearestAlarms);
        void restoreOriginalAlarmTime(std::vector<AlarmsRecord> &nearestAlarms);
        std::vector<AlarmsRecord> getPossibleNearestAlarms(std::vector<AlarmsRecord> &allRecords);

      public:
        explicit AlarmsTimeEvents(sys::Service *service);
        ~AlarmsTimeEvents() = default;
    };

} // namespace stm
