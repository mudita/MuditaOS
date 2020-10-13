#include "ParserICS.hpp"
#include <module-apps/application-calendar/data/dateCommon.hpp>

auto ParserICS::updateICS(const std::string &data) -> void
{
    std::ofstream file(this->icsFile);
    file << data;
    file.close();
}

auto ParserICS::stringFromICS() -> std::string
{
    std::ifstream fileStream("events.ics");
    std::string content((std::istreambuf_iterator<char>(fileStream)), (std::istreambuf_iterator<char>()));
    return content;
}

auto ParserICS::reminderToICS(uint32_t reminder) -> list<Alarm> *
{
    auto alarm = new Alarm;
    if (reminder != 0xFFFF) {
        alarm->Active        = true;
        alarm->Trigger.Value = reminder;
    }

    std::list<Alarm> *alarms = new std::list<Alarm>();
    alarms->push_back(*alarm);

    return alarms;
}

auto ParserICS::reminderFromICS(list<Alarm> *alarms) -> uint32_t
{
    auto alarm        = alarms->back();
    uint32_t reminder = 0xFF;
    if (alarm.Active) {
        reminder = alarm.Trigger.Value;
    }

    return reminder;
}

auto ParserICS::repeatOptionToICS(uint32_t repeat) -> Recurrence
{
    auto rule = Recurrence();
    switch (Repeat(repeat)) {
    case Repeat::never: {
        return rule;
    }
    case Repeat::daily: {
        rule.Freq     = DAY;
        rule.Interval = 1;
        rule.Count    = 7;
        return rule;
    }
    case Repeat::weekly: {
        rule.Freq     = WEEK;
        rule.Interval = 1;
        rule.Count    = 4;
        return rule;
    }
    case Repeat::two_weeks: {
        rule.Freq     = WEEK;
        rule.Interval = 2;
        rule.Count    = 4;
        return rule;
    }
    case Repeat::month: {
        rule.Freq     = MONTH;
        rule.Interval = 1;
        rule.Count    = 12;
        return rule;
    }
    case Repeat::year: {
        rule.Freq     = YEAR;
        rule.Interval = 1;
        rule.Count    = 4;
        return rule;
    }
    default: {
        /// Custom repeat
        rule.Interval = repeat;
        return rule;
    }
    }
}
auto ParserICS::repeatOptionFromICS(const Recurrence &rule) -> uint32_t
{
    switch (TimeUnit(rule.Freq)) {
    case DAY: {
        return static_cast<uint32_t>(Repeat::daily);
    }
    case WEEK: {
        if (rule.Interval == 1) {
            return static_cast<uint32_t>(Repeat::weekly);
        }
        else if (rule.Interval == 2) {
            return static_cast<uint32_t>(Repeat::two_weeks);
        }
        else {
            LOG_ERROR("Wrong Event Rule interval value provided from ICS");
            assert(false);
            return 0;
        }
    }
    case MONTH: {
        return static_cast<uint32_t>(Repeat::month);
    }
    case YEAR: {
        if (rule.Interval == 1) {
            return static_cast<uint32_t>(Repeat::year);
        }
        else if (rule.Interval == 0) {
            return static_cast<uint32_t>(Repeat::never);
        }
        else {
            /// Custom repeat
            return rule.Interval;
        }
    }
    default: {
        LOG_ERROR("Application Desktop provided wrong event frequency value");
        assert(false);
        return 0;
    }
    }
}

auto ParserICS::eventRecordsToICS(std::unique_ptr<std::vector<EventsRecord>> records) -> void
{
    for (auto currentRecord : *records) {
        auto currentEvent     = new Event();
        currentEvent->UID     = currentRecord.UID;
        currentEvent->Summary = currentRecord.title;
        currentEvent->DtStart = TimePointToString(currentRecord.date_from);
        currentEvent->DtEnd   = TimePointToString(currentRecord.date_till);
        currentEvent->RRule   = repeatOptionToICS(currentRecord.repeat);
        currentEvent->Alarms  = reminderToICS(currentRecord.reminder);
        Calendar->AddEvent(currentEvent);
    }
    return;
}

auto ParserICS::eventRecordsFromICS() -> std::vector<EventsRecord>
{
    Event *CurrentEvent;
    ICalendar::Query SearchQuery(this->Calendar.get());
    SearchQuery.ResetPosition();

    std::vector<EventsRecord> records;

    while ((CurrentEvent = SearchQuery.GetNextEvent(false)) != NULL) {
        EventsRecord currentRecord;
        currentRecord.title     = CurrentEvent->Summary;
        currentRecord.date_from = TimePointFromString(CurrentEvent->DtStart.Format().c_str());
        currentRecord.date_till = TimePointFromString(CurrentEvent->DtEnd.Format().c_str());
        currentRecord.repeat    = repeatOptionFromICS(CurrentEvent->RRule);
        currentRecord.reminder  = reminderFromICS(CurrentEvent->Alarms);

        records.push_back(currentRecord);
    }
    return records;
}

auto ParserICS::getEventByDatabaseID(uint32_t ID) -> EventsRecord
{
    Event *CurrentEvent;
    ICalendar::Query SearchQuery(this->Calendar.get());
    SearchQuery.ResetPosition();

    EventsRecord currentRecord;

    while ((CurrentEvent = SearchQuery.GetNextEvent(false)) != NULL ||
           CurrentEvent->Description == std::to_string(ID)) {}
    assert(CurrentEvent != NULL);

    currentRecord.title     = CurrentEvent->Summary;
    currentRecord.date_from = TimePointFromString(CurrentEvent->DtStart.Format().c_str());
    currentRecord.date_till = TimePointFromString(CurrentEvent->DtEnd.Format().c_str());
    currentRecord.repeat    = repeatOptionFromICS(CurrentEvent->RRule);
    currentRecord.reminder  = reminderFromICS(CurrentEvent->Alarms);

    return currentRecord;
}

auto ParserICS::updateEvent(Event &event) -> EventsRecord
{
    Event *CurrentEvent;
    ICalendar::Query SearchQuery(this->Calendar.get());
    SearchQuery.ResetPosition();

    while ((CurrentEvent = SearchQuery.GetNextEvent(false)) != NULL || CurrentEvent->Description == event.Description) {
    }
    assert(CurrentEvent != NULL);

    CurrentEvent->Summary = event.Summary;
    CurrentEvent->DtStart = event.DtStart;
    CurrentEvent->DtEnd   = event.DtEnd;
    CurrentEvent->RRule   = event.RRule;
    CurrentEvent->Alarms  = event.Alarms;

    Calendar->ModifyEvent(CurrentEvent);

    EventsRecord currentRecord;
    currentRecord.title     = CurrentEvent->Summary;
    currentRecord.date_from = TimePointFromString(CurrentEvent->DtStart.Format().c_str());
    currentRecord.date_till = TimePointFromString(CurrentEvent->DtEnd.Format().c_str());
    currentRecord.repeat    = repeatOptionFromICS(CurrentEvent->RRule);
    currentRecord.reminder  = reminderFromICS(CurrentEvent->Alarms);

    return currentRecord;
}