// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarTimeEvents.hpp"

#include <module-services/service-appmgr/ApplicationManager.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-services/service-db/messages/DBNotificationMessage.hpp>
#include <module-db/queries/calendar/QueryEventsSelectFirstUpcoming.hpp>

#include <module-gui/gui/SwitchData.hpp>
#include <module-apps/application-calendar/data/CalendarData.hpp>
#include <module-apps/application-calendar/ApplicationCalendar.hpp>
#include <module-apps/application-calendar/data/dateCommon.hpp>

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

        sapm::ApplicationManager::messageSwitchApplication(
            service(), app::name_calendar, style::window::calendar::name::event_reminder_window, std::move(eventData));
    }
} // namespace stm
