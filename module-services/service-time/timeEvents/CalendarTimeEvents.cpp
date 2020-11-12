// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarTimeEvents.hpp"

#include <service-appmgr/Controller.hpp>                                 // for Controller
#include <service-db/DBServiceAPI.hpp>                                   // for DBServiceAPI
#include <module-db/queries/calendar/QueryEventsSelectFirstUpcoming.hpp> // for SelectFirstUpcoming, SelectFirstUpcomingResult
#include <module-gui/gui/SwitchData.hpp>                                 // for SwitchData
#include <module-apps/application-calendar/data/CalendarData.hpp>        // for EventRecordData
#include <module-apps/application-calendar/ApplicationCalendar.hpp> // for name_calendar
#include <module-apps/application-calendar/data/dateCommon.hpp>     // for TimePointNow, TimePoint, TIME_POINT_INVALID
#include <algorithm>                                                // for min
#include <chrono> // for duration, milliseconds, operator-, operator<, time_point, duration_cast, operator!=, minutes, operator""ms
#include <type_traits> // for __success_type<>::type
#include <utility>     // for move
#include <vector>      // for vector

#include "BaseInterface.hpp"                              // for Interface, Interface::Name, Interface::Name::Events
#include "Common/Query.hpp"                               // for Query, QueryResult
#include "application-calendar/widgets/CalendarStyle.hpp" // for event_reminder_window
#include "queries/calendar/QueryEventsEdit.hpp"           // for Edit
#include "service-time/timeEvents/TimeEvents.hpp"         // for TimeEvents

namespace sys
{
    class Service;
} // namespace sys

namespace stm
{
    constexpr static auto eventTimerMinSkipInterval = 100ms;

    CalendarTimeEvents::CalendarTimeEvents(sys::Service *service) : TimeEvents(service)
    {}

    bool CalendarTimeEvents::sendNextEventQuery()
    {
        TimePoint filterFrom = TimePointNow();
        TimePoint filterTill = filterFrom;
        if (startTP != TIME_POINT_INVALID) {
            filterFrom = std::min(startTP, filterFrom);
            filterTill = filterFrom;
        }

        return DBServiceAPI::GetQuery(service(),
                                      db::Interface::Name::Events,
                                      std::make_unique<db::query::events::SelectFirstUpcoming>(filterFrom, filterTill));
    }

    uint32_t CalendarTimeEvents::calcToNextEventInterval(std::unique_ptr<db::QueryResult> nextEventQueryResult)
    {
        auto firstUpcomingQuery =
            dynamic_cast<db::query::events::SelectFirstUpcomingResult *>(nextEventQueryResult.get());
        if (firstUpcomingQuery == nullptr) {
            return 0;
        }

        std::unique_ptr<std::vector<EventsRecord>> records = firstUpcomingQuery->getResult();
        if (records->size() == 0) {
            return 0;
        }

        eventRecord   = records->at(0);
        startTP       = eventRecord.date_from - minutes{eventRecord.reminder};
        auto duration = eventRecord.date_from - std::chrono::minutes{eventRecord.reminder} - TimePointNow();
        if (duration.count() <= 0) {
            duration = std::chrono::milliseconds(eventTimerMinSkipInterval);
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    bool CalendarTimeEvents::sendEventFiredQuery()
    {
        eventRecord.reminder_fired = TimePointNow();
        return DBServiceAPI::GetQuery(
            service(), db::Interface::Name::Events, std::make_unique<db::query::events::Edit>(eventRecord));
    }

    void CalendarTimeEvents::invokeEvent()
    {
        std::unique_ptr<EventRecordData> eventData = std::make_unique<EventRecordData>();
        eventData->setDescription(style::window::calendar::name::event_reminder_window);
        auto event = std::make_shared<EventsRecord>(eventRecord);
        eventData->setData(event);

        app::manager::Controller::switchApplication(
            service(), app::name_calendar, style::window::calendar::name::event_reminder_window, std::move(eventData));
    }
} // namespace stm
