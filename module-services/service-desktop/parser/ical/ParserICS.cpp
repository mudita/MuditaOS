#include "ParserICS.hpp"
#include <module-apps/application-calendar/data/dateCommon.hpp>

auto ParserICS::updateICS(json11::Json body) -> void
{
    std::string icsString = body.string_value();
    std::ofstream file(this->icsFile);
    file << icsString;
    file.close();
}

// auto ParserICS::createUID() -> std::string
//{
//    auto tempEvent = std::make_unique<Event>();
//    char Temp[16];
//    string Line;
//    streamoff Offset;
//
//    tempEvent->DtStamp.SetToNow();
//    tempEvent->UID = NewEvent->DtStamp;
//    tempEvent->UID += '-';
//    sprintf(Temp, "%d", rand());
//    tempEvent->UID += Temp;
//
//    return tempEvent->UID;
//}

// auto ParserICS::stringToICS(json11::Json body) -> void
//{
//    std::string icsString = body.string_value();
//    std::ofstream file(this->icsFile);
//    file << icsString;
//    file.close();
//}

auto ParserICS::stringFromICS() -> std::string
{
    /// TODO: Maybe not needed (check text conversion to object json)
    /// OR Implement string from file
    return "";
}

auto ParserICS::reminderToICS(uint32_t reminder) -> list<Alarm> *
{
    auto alarm = new Alarm;
    if (reminder != 0xFFFF) {
        alarm->Active        = true;
        alarm->Trigger.Value = reminder;
    }

    std::list<Alarm> alarms;
    alarms.push_back(alarm);

    return &alarms;
}

auto ParserICS::reminderFromICS(list<Alarm> *alarms) -> uint32_t
{
    auto alarm        = alarms->back();
    uint32_t reminder = 0xFF;
    if (alarm->active) {
        reminder = alarm.Trigger.Value;
    }

    return reminder;
}

auto ParserICS::repeatOptionToICS(uint32_t repeat) -> Recurrence
{
    auto rule = Recurrence();
    switch (repeat) {
    case Repeat::never: {
        return rule;
    }
    case Repeat::daily: {
        rule.Freq     = DAY;
        rule.Interval = 1;
        rule.Count(7);
        return rule;
    }
    case Repeat::weekly: {
        rule.Freq     = WEEK;
        rule.Interval = 1;
        rule.Count(4);
        return rule;
    }
    case Repeat::two_weeks: {
        rule.Freq     = WEEK;
        rule.Interval = 2;
        rule.Count(4);
    }
    case Repeat::monthly: {
        rule.Freq     = MONTH;
        rule.Interval = 1;
        rule.Count(12);
        return rule;
    }
    case Repeat::year: {
        rule.Freq     = YEAR;
        rule.Interval = 1;
        rule.Count(4);
        return rule;
    }
    default: {
        /// Custom repeat
        rule.Interval = repeat;
    }
    }
}
auto ParserICS::repeatOptionFromICS(const Recurrence &rule) -> uint32_t
{
    switch (rule.Freq) {
    case Repeat::never: {
        return Repeat::daily;
    }
    case DAY: {
        return Repeat::daily;
    }
    case WEEK: {
        if (rule.Interval == 1) {
            return Repeat::weekly;
        }
        else if (rule.Interval == 2) {
            return Repeat::two_weeks;
        }
    }
    case MONTH: {
        return Repeat::month;
    }
    case YEAR: {
        if (rule.Interval == 1) {
            return Repeat::year;
        }
        else if (rule.Interval == 0) {
            return Repeat::never;
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
        currentEvent->UID     = currentEvent.UID;
        currentEvent->Summary = currentRecord.title;
        currentEvent->DtStart = TimePointToString(currentRecord.date_from);
        currentEvent->DtEnd   = TimePointToString(currentRecord.date_till);
        currentEvent->RRule   = repeatOptionToICS(currentRecord.repeat);
        currentEvent->Alarms  = reminderToICS(currentRecord.reminder);
        Calendar.AddEvent(currentEvent);
    }
    return;
}

auto ParserICS::eventRecordsFromICS() -> std::vector<EventsRecord>
{
    Event *CurrentEvent;
    ICalendar::Query SearchQuery(&this->Calendar);
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
    ICalendar::Query SearchQuery(&this->Calendar);
    SearchQuery.ResetPosition();

    while ((CurrentEvent = SearchQuery.GetNextEvent(false)) != NULL ||
           currentRecord->Description == std::to_string(ID)) {}
    assert(CurrentEvent != NULL);

    EventsRecord currentRecord;
    currentRecord.title     = CurrentEvent->Summary;
    currentRecord.date_from = TimePointFromString(CurrentEvent->DtStart.Format().c_str());
    currentRecord.date_till = TimePointFromString(CurrentEvent->DtEnd.Format().c_str());
    currentRecord.repeat    = repeatOptionFromICS(CurrentEvent->RRule);
    currentRecord.reminder  = reminderFromICS(CurrentEvent->Alarms);

    return currentRecord;
}

auto ParserICS::updateEvent(Event &event) -> EventsRecord
{
    ICalendar::Query SearchQuery(&this->Calendar);
    SearchQuery.ResetPosition();

    while ((CurrentEvent = SearchQuery.GetNextEvent(false)) != NULL || CurrentEvent->Description == event.Description) {
    }
    assert(CurrentEvent != NULL);

    CurrentEvent->Summary = event.Summary;
    CurrentEvent->DtStart = event.DtStart;
    CurrentEvent->DtEnd   = event.DtEnd;
    CurrentEvent->RRule   = event.PRule;
    CurrentEvent->Alarms  = event.Alarms;

    Calendar->ModifyEvent(currentEvent);

    EventsRecord currentRecord;
    currentRecord.ID        = std::atoi(CurrentEvent->Description);
    currentRecord.title     = CurrentEvent->Summary;
    currentRecord.date_from = TimePointFromString(CurrentEvent->DtStart.Format().c_str());
    currentRecord.date_till = TimePointFromString(CurrentEvent->DtEnd.Format().c_str());
    currentRecord.repeat    = repeatOptionFromICS(CurrentEvent->RRule);
    currentRecord.reminder  = reminderFromICS(CurrentEvent->Alarms);

    return currentRecord;
}