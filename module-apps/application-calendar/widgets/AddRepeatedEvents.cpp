//
// Created by tomasz on 24.08.2020.
//
#include "AddRepeatedEvents.hpp"
#include <module-db/Interface/EventsRecord.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-db/queries/calendar/QueryEventsAdd.hpp>
#include <module-utils/date/include/date/dateCommon.h>

void AddRepeatedEvents::addDaily(std::shared_ptr<EventsRecord> event)
{
    auto record = event.get();
    for (uint32_t i = 1; i <= 7; i++) {
        record->date_from += date::days{1};
        record->date_from += date::days{1};
        DBServiceAPI::GetQuery(
            application, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(*record));
    }
}

void AddRepeatedEvents::addWeekly(std::shared_ptr<EventsRecord> event)
{
    auto record = event.get();
    for (uint32_t i = 1; i <= 4; i++) {
        record->date_from += date::days{7};
        record->date_from += date::days{7};
        DBServiceAPI::GetQuery(
            application, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(*record));
    }
}
void AddRepeatedEvents::addTwoWeeks(std::shared_ptr<EventsRecord> event)
{
    auto record = event.get();
    for (uint32_t i = 1; i <= 4; i++) {
        record->date_from += date::days{14};
        record->date_from += date::days{14};
        DBServiceAPI::GetQuery(
            application, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(*record));
    }
}
void AddRepeatedEvents::addMonth(std::shared_ptr<EventsRecord> event)
{
    auto record = event.get();
    for (uint32_t i = 1; i <= 4; i++) {
        record->date_from += date::months{1};
        record->date_from += date::months{1};
        DBServiceAPI::GetQuery(
            application, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(*record));
    }
}
void AddRepeatedEvents::addYear(std::shared_ptr<EventsRecord> event)
{
    auto record = event.get();
    for (uint32_t i = 1; i <= 4; i++) {
        record->date_from += date::years{1};
        record->date_from += date::years{1};
        DBServiceAPI::GetQuery(
            application, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(*record));
    }
}
void AddRepeatedEvents::addCustom(std::shared_ptr<EventsRecord> event)
{

    //    auto record = event.get();
    //    for (uint32_t i = 1; i <= 7; i++) {
    //        record->date_from = record->date_from + 10000 * i;
    //        record->date_from = record->date_from + 10000 * i;
    //        DBServiceAPI::GetQuery(
    //            application, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(*record));
    //    }
}

void AddRepeatedEvents::addRepeatedEvents(std::shared_ptr<EventsRecord> event)
{
    switch (event->repeat) {
    case Daily: {
        addDaily(event);
        break;
    }
    case Weekly: {
        addWeekly(event);
        break;
    }
    case TwoWeeks: {
        addTwoWeeks(event);
        break;
    }
    case Month: {
        addMonth(event);
        break;
    }
    case Year: {
        addYear(event);
        break;
    }
    case Custom: {
        addCustom(event);
        break;
    }
    }
}