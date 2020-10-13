#pragma once
#include <module-utils/icalendarlib/icalendar.h>

class ParserICS
{
    std::unique_ptr<ICalendar> Calendar;
    const char *icsFile = "events.ics";

  public:
    ParserICS()
    {
        this->Calendar = std::make_unique<ICalendar>(this->icsFile);
    };

    auto updateICS(json11::Json body) -> void;
    //    auto createUID() -> std::string;
    auto stringFromICS() -> std::string;

    auto reminderToICS(uint32_t reminder) -> list<Alarm> *;
    auto reminderFromICS(list<Alarm> *alarm) -> uint32_t;
    auto repeatOptionToICS(uint32_t repeat) -> Recurrence;
    auto repeatOptionFromICS(const Recurrence &frequency) -> uint32_t;

    auto eventRecordsToICS(std::unique_ptr<std::vector<EventsRecord>> records) -> void;
    auto eventRecordsFromICS() -> std::vector<EventsRecord>;

    auto getEventByDatabaseID(uint32_t ID) -> EventsRecord;
    auto updateEvent(Event &event) -> EventsRecord;
};
