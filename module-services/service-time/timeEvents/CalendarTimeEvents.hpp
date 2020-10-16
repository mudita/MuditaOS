#pragma once
#include "TimeEvents.hpp"
#include <module-db/queries/calendar/QueryEventsEdit.hpp>

namespace stm
{
    class CalendarTimeEvents : public TimeEvents
    {
      private:
        EventsRecord eventRecord;
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
