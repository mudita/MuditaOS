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
        const std::string TimerName() override
        {
            return "CalendarTimeEvents_timer";
        };
        uint32_t CalcToNextEventInterval(std::unique_ptr<db::QueryResult> nextEventQueryResult) override;

        bool SendNextEventQuery() override;
        bool SendEventFiredQuery() override;
        void InvokeEvent() override;

      public:
        CalendarTimeEvents() = delete;
        CalendarTimeEvents(sys::Service *service);
        ~CalendarTimeEvents() = default;
    };

} // namespace stm
