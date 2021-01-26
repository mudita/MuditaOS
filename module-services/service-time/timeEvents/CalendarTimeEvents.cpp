// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-time/CalendarTimeEvents.hpp>
#include <service-time/TimeEvents.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <application-calendar/widgets/CalendarStyle.hpp>
#include <module-apps/application-calendar/ApplicationCalendar.hpp>
#include <module-apps/application-calendar/data/CalendarData.hpp>
#include <time/time_conversion.hpp>
#include <module-db/queries/calendar/QueryEventsSelectFirstUpcoming.hpp>
#include <module-gui/gui/SwitchData.hpp>
#include <queries/calendar/QueryEventsEdit.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBServiceAPI.hpp>

#include <algorithm>
#include <chrono>
#include <type_traits>
#include <utility>
#include <vector>

namespace sys
{
    class Service;
} // namespace sys

namespace stm
{
    using namespace std::chrono_literals;
    constexpr static auto eventTimerMinSkipInterval = 100ms;

    CalendarTimeEvents::CalendarTimeEvents(sys::Service *service) : TimeEvents(service)
    {}

    bool CalendarTimeEvents::sendNextEventQuery()
    {
        utils::time::TimePoint filterFrom = utils::time::TimePointNow();
        utils::time::TimePoint filterTill = filterFrom;
        if (startTP != utils::time::TIME_POINT_INVALID) {
            filterFrom = std::min(startTP, filterFrom);
            filterTill = filterFrom;
        }

        const auto [succeed, _] =
            DBServiceAPI::GetQuery(service(),
                                   db::Interface::Name::Events,
                                   std::make_unique<db::query::events::SelectFirstUpcoming>(filterFrom, filterTill));
        return succeed;
    }

    uint32_t CalendarTimeEvents::calcToNextEventInterval(std::unique_ptr<db::QueryResult> nextEventQueryResult)
    {
        auto firstUpcomingQuery =
            dynamic_cast<db::query::events::SelectFirstUpcomingResult *>(nextEventQueryResult.get());
        if (firstUpcomingQuery == nullptr) {
            return 0;
        }

        std::vector<EventsRecord> records = firstUpcomingQuery->getResult();
        if (records.size() == 0) {
            return 0;
        }

        eventRecord   = records.at(0);
        startTP       = eventRecord.date_from - std::chrono::minutes{eventRecord.reminder};
        auto duration = eventRecord.date_from - std::chrono::minutes{eventRecord.reminder} - utils::time::TimePointNow();
        if (duration.count() <= 0) {
            duration = std::chrono::milliseconds(eventTimerMinSkipInterval);
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    bool CalendarTimeEvents::sendEventFiredQuery()
    {
        eventRecord.reminder_fired = utils::time::TimePointNow();
        const auto [succeed, _]    = DBServiceAPI::GetQuery(
            service(), db::Interface::Name::Events, std::make_unique<db::query::events::Edit>(eventRecord));
        return succeed;
    }

    void CalendarTimeEvents::invokeEvent()
    {
        std::unique_ptr<EventRecordData> eventData = std::make_unique<EventRecordData>();
        eventData->setDescription(style::window::calendar::name::event_reminder_window);
        auto event = std::make_shared<EventsRecord>(eventRecord);
        eventData->setData(event);

        app::manager::Controller::sendAction(service(), app::manager::actions::ShowReminder, std::move(eventData));
    }
} // namespace stm
