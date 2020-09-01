#include "AddRepeatedEvents.hpp"
#include <module-db/Interface/EventsRecord.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-db/queries/calendar/QueryEventsAdd.hpp>
#include <module-apps/application-calendar/data/dateCommon.h>
#include "../data/OptionParser.hpp"

void AddRepeatedEvents::addDaily(std::shared_ptr<EventsRecord> event)
{
    auto record = event.get();
    for (uint32_t i = 1; i < 7; i++) {
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
    auto record                = event.get();
    uint32_t startWeekdayIndex = WeekdayIndexFromTimePoint(record->date_from);

    // Get table of weekdays to repeat
    OptionParser parser;
    auto weekDayRepeatData = std::make_unique<WeekDaysRepeatData>();
    assert(weekDayRepeatData != nullptr);
    auto weekDayData = parser.setWeekDayOptions(record->repeat, std::move(weekDayRepeatData));

    // Count num of weekdays to repeat
    uint32_t weekDayNum = 0;
    for (uint32_t i = 0; i < 7; i++) {
        if (weekDayData->getData(i))
            weekDayNum++;
    }
    // Num of entries to add
    uint32_t entriesToAdd = weekDayNum * 4;

    uint32_t currWeekdayIndex = startWeekdayIndex;
    // Add all entries
    while (entriesToAdd > 0) {
        if (weekDayData->getData(currWeekdayIndex)) {
            DBServiceAPI::GetQuery(
                application, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(*record));

            entriesToAdd--;
        }

        record->date_from += date::days{1};
        record->date_till += date::days{1};
        currWeekdayIndex = (currWeekdayIndex + 1) % 7;
    }
}

void AddRepeatedEvents::addRepeatedEvents(std::shared_ptr<EventsRecord> event)
{
    switch (event->repeat) {
    case static_cast<unsigned int>(Repeat::Daily): {
        addDaily(event);
        break;
    }
    case static_cast<unsigned int>(Repeat::Weekly): {
        addWeekly(event);
        break;
    }
    case static_cast<unsigned int>(Repeat::TwoWeeks): {
        addTwoWeeks(event);
        break;
    }
    case static_cast<unsigned int>(Repeat::Month): {
        addMonth(event);
        break;
    }
    case static_cast<unsigned int>(Repeat::Year): {
        addYear(event);
        break;
    }
    default: {
        if (event->repeat > 0)
            addCustom(event);
        break;
    }
    }
}