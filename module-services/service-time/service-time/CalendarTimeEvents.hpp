// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "TimeEvents.hpp"

#include <application-calendar/data/dateCommon.hpp>
#include <module-db/Interface/EventsRecord.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>

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
    class CalendarTimeEvents : public TimeEvents
    {
      private:
        std::vector<EventsRecord> eventRecords;
        TimePoint startTP = TIME_POINT_INVALID;

      protected:
        const std::string timerName() override
        {
            return "CalendarTimeEvents_timer";
        };
        uint32_t calcToNextEventInterval(std::unique_ptr<db::QueryResult> nextEventQueryResult) override;

        bool sendNextEventQuery() override;
        bool sendEventFiredQuery() override;
        void invokeEvent() override;

      public:
        CalendarTimeEvents() = delete;
        explicit CalendarTimeEvents(sys::Service *service);
        ~CalendarTimeEvents() = default;
    };

} // namespace stm
