// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventsTable.hpp"
#include "module-db/Interface/EventsRecord.hpp"
//#include <module-apps/application-calendar/widgets/CalendarStyle.hpp>
//#include <module-apps/application-calendar/data/OptionParser.hpp>

#include <log/log.hpp>
#include <Utils.hpp>

#include <cassert>

EventsTable::EventsTable(Database *db) : Table(db)
{}

bool EventsTable::create()
{
    if (!db->execute(createTableQuery)) {
        return false;
    }

    return true;
}

bool EventsTable::add(EventsTableRow entry)
{
    // Prevent duplicates using ANDs:
    if (entry.UID.empty()) {
        entry.UID = createUID();
    }
    return db->execute("INSERT or IGNORE INTO events (uid, title, date_from, date_till, reminder, repeat, "
                       "reminder_fired, provider_type, provider_id, provider_iCalUid) "
                       "SELECT '%q','%q', '%q','%q', %lu, %lu, '%q','%q', '%q','%q'"
                       "WHERE NOT EXISTS "
                       "(SELECT 1 FROM events e "
                       "WHERE e.title='%q' "
                       "AND e.date_from='%q' "
                       "AND e.date_till='%q' "
                       "AND e.reminder=%lu "
                       "AND e.repeat=%lu );",
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat);
}

bool EventsTable::addDaily(EventsTableRow entry)
{
    if (entry.UID.empty()) {
        entry.UID = createUID();
    }
    return db->execute("INSERT or IGNORE INTO events (uid, title, date_from, date_till, reminder, repeat, "
                       "reminder_fired, provider_type, provider_id, provider_iCalUid) VALUES"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q');",
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{1}).c_str(),
                       TimePointToString(entry.date_till + date::days{1}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{2}).c_str(),
                       TimePointToString(entry.date_till + date::days{2}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{3}).c_str(),
                       TimePointToString(entry.date_till + date::days{3}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{4}).c_str(),
                       TimePointToString(entry.date_till + date::days{4}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{5}).c_str(),
                       TimePointToString(entry.date_till + date::days{5}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{6}).c_str(),
                       TimePointToString(entry.date_till + date::days{6}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str());
}

bool EventsTable::addWeekly(EventsTableRow entry)
{
    if (entry.UID.empty()) {
        entry.UID = createUID();
    }
    return db->execute("INSERT or IGNORE INTO events (uid, title, date_from, date_till, reminder, repeat, "
                       "reminder_fired, provider_type, provider_id, provider_iCalUid) VALUES"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q');",
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{7}).c_str(),
                       TimePointToString(entry.date_till + date::days{7}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{14}).c_str(),
                       TimePointToString(entry.date_till + date::days{14}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{21}).c_str(),
                       TimePointToString(entry.date_till + date::days{21}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str());
}

bool EventsTable::addTwoWeeks(EventsTableRow entry)
{
    if (entry.UID.empty()) {
        entry.UID = createUID();
    }
    return db->execute("INSERT or IGNORE INTO events (uid, title, date_from, date_till, reminder, repeat, "
                       "reminder_fired, provider_type, provider_id, provider_iCalUid) VALUES"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q');",
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{14}).c_str(),
                       TimePointToString(entry.date_till + date::days{14}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{28}).c_str(),
                       TimePointToString(entry.date_till + date::days{28}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from + date::days{42}).c_str(),
                       TimePointToString(entry.date_till + date::days{42}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str());
}

bool EventsTable::addMonth(EventsTableRow entry)
{
    if (entry.UID.empty()) {
        entry.UID = createUID();
    }
    return db->execute("INSERT or IGNORE INTO events (uid, title, date_from, date_till, reminder, repeat, "
                       "reminder_fired, provider_type, provider_id, provider_iCalUid) VALUES"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q');",
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{1}).c_str(),
                       TimePointToString(entry.date_till, date::months{1}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{2}).c_str(),
                       TimePointToString(entry.date_till, date::months{2}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{3}).c_str(),
                       TimePointToString(entry.date_till, date::months{3}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{4}).c_str(),
                       TimePointToString(entry.date_till, date::months{4}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{5}).c_str(),
                       TimePointToString(entry.date_till, date::months{5}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{6}).c_str(),
                       TimePointToString(entry.date_till, date::months{6}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{7}).c_str(),
                       TimePointToString(entry.date_till, date::months{7}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{8}).c_str(),
                       TimePointToString(entry.date_till, date::months{8}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{9}).c_str(),
                       TimePointToString(entry.date_till, date::months{9}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{10}).c_str(),
                       TimePointToString(entry.date_till, date::months{10}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{11}).c_str(),
                       TimePointToString(entry.date_till, date::months{11}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::months{12}).c_str(),
                       TimePointToString(entry.date_till, date::months{12}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str());
}

bool EventsTable::addYear(EventsTableRow entry)
{
    if (entry.UID.empty()) {
        entry.UID = createUID();
    }
    return db->execute("INSERT or IGNORE INTO events (uid, title, date_from, date_till, reminder, repeat, "
                       "reminder_fired, provider_type, provider_id, provider_iCalUid) VALUES"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q'),"
                       "('%q','%q', '%q','%q', %u, %u,'%q', '%q', '%q', '%q');",
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::years{1}).c_str(),
                       TimePointToString(entry.date_till, date::years{1}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::years{2}).c_str(),
                       TimePointToString(entry.date_till, date::years{2}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::years{3}).c_str(),
                       TimePointToString(entry.date_till, date::years{3}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str(),
                       entry.title.c_str(),
                       TimePointToString(entry.date_from, date::years{4}).c_str(),
                       TimePointToString(entry.date_till, date::years{4}).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str());
}

std::vector<bool> EventsTable::parseOptions(uint32_t repeatOptionValue)
{
    const uint32_t startBit        = 16;
    const uint32_t numberOfOptions = 7;
    std::vector<bool> weekDayOptions;
    for (uint32_t i = startBit; i < startBit + numberOfOptions; i++) {
        weekDayOptions.push_back(false);
    }
    for (uint32_t i = startBit; i < startBit + numberOfOptions; i++) {
        if (repeatOptionValue & (1 << i)) {
            LOG_DEBUG("Set option array %d", static_cast<int>(i));
            weekDayOptions[i - startBit] = true;
        }
    }
    return weekDayOptions;
}

bool EventsTable::addCustom(EventsTableRow entry)
{
    // OptionParser parser;
    const uint32_t numberOfWeeks    = 4;
    const uint32_t numberOfWeekDays = 7;
    bool result                     = true;
    std::vector<bool> weekDayOptions;
    weekDayOptions          = parseOptions(entry.repeat);
    uint32_t incrementation = 0;

    result = result && db->execute("INSERT or IGNORE INTO events (uid, title, date_from, date_till, reminder, repeat, "
                                   "reminder_fired, provider_type, provider_id, provider_iCalUid) VALUES"
                                   "('%q','%q', '%q','%q', %u, %u, '%q','%q', '%q','%q');",
                                   entry.UID.c_str(),
                                   entry.title.c_str(),
                                   TimePointToString(entry.date_from).c_str(),
                                   TimePointToString(entry.date_till).c_str(),
                                   entry.reminder,
                                   entry.repeat,
                                   TimePointToString(entry.reminder_fired).c_str(),
                                   entry.provider_type.c_str(),
                                   entry.provider_id.c_str(),
                                   entry.provider_iCalUid.c_str());

    auto dateFrom = getFirstWeekDay(entry.date_from);
    auto dateTill = getFirstWeekDay(entry.date_till);
    for (uint32_t i = 1; i <= numberOfWeeks; i++) {
        for (auto option : weekDayOptions) {
            if (option) {
                LOG_DEBUG("start: %s", TimePointToString(dateFrom + date::days{incrementation}).c_str());
                LOG_DEBUG("start: %s", TimePointToString(dateTill + date::days{incrementation}).c_str());
                result =
                    result &&
                    db->execute("INSERT or IGNORE INTO events (uid, title, date_from, date_till, "
                                "reminder, repeat, reminder_fired, provider_type, provider_id, provider_iCalUid) VALUES"
                                "('%q','%q', '%q','%q', %u, %u, '%q','%q', '%q','%q');",
                                entry.UID.c_str(),
                                entry.title.c_str(),
                                TimePointToString(dateFrom + date::days{incrementation}).c_str(),
                                TimePointToString(dateTill + date::days{incrementation}).c_str(),
                                entry.reminder,
                                entry.repeat,
                                TimePointToString(entry.reminder_fired).c_str(),
                                entry.provider_type.c_str(),
                                entry.provider_id.c_str(),
                                entry.provider_iCalUid.c_str());
            }
            ++incrementation;
        }
        incrementation = i * numberOfWeekDays;
    }
    return result;
}

bool EventsTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM events where _id = %u;", id);
}

bool EventsTable::removeByUID(const std::string &UID)
{
    return db->execute("DELETE FROM events where uid = '%q';", UID.c_str());
}

bool EventsTable::removeByField(EventsTableFields field, const char *str)
{
    std::string fieldName;

    switch (field) {
    case EventsTableFields::date_from: {
        fieldName = "date_from";
        break;
    }
    case EventsTableFields::date_till: {
        fieldName = "date_till";
        break;
    }
    }

    return db->execute("DELETE FROM events where %q = '%q';", fieldName.c_str(), str);
}

bool EventsTable::update(EventsTableRow entry)
{
    return db->execute("UPDATE events SET title= '%q', date_from = '%q', date_till = '%q', reminder "
                       "= %u, repeat = %u, reminder_fired = '%q', provider_type = '%q', provider_id = '%q', "
                       "provider_iCalUid = '%q' WHERE _id = %u;",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       entry.repeat,
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.ID);
}

bool EventsTable::updateByUID(EventsTableRow entry)
{
    return db->execute("UPDATE events SET title= '%q', date_from = '%q', date_till = '%q', reminder "
                       "= %u, repeat = %u, reminder_fired = '%q', provider_type = '%q', provider_id = '%q', "
                       "provider_iCalUid = '%q' WHERE uid = '%q';",
                       entry.title.c_str(),
                       TimePointToString(entry.date_from).c_str(),
                       TimePointToString(entry.date_till).c_str(),
                       entry.reminder,
                       static_cast<uint32_t>(entry.repeat),
                       TimePointToString(entry.reminder_fired).c_str(),
                       entry.provider_type.c_str(),
                       entry.provider_id.c_str(),
                       entry.provider_iCalUid.c_str(),
                       entry.UID.c_str());
}

EventsTableRow EventsTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM events WHERE _id= %u;", id);

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return EventsTableRow();
    }

    assert(retQuery->getRowCount() == 1);

    return EventsTableRow{
        (*retQuery)[0].getUInt32(),                              // ID
        (*retQuery)[1].getString(),                              // UID
        (*retQuery)[2].getString(),                              // title
        TimePointFromString((*retQuery)[3].getString().c_str()), // date_from
        TimePointFromString((*retQuery)[4].getString().c_str()), // date_till
        (*retQuery)[5].getUInt32(),                              // reminder
        (*retQuery)[6].getUInt32(),                              // repeat
        TimePointFromString((*retQuery)[7].getString().c_str()), // reminder_fired
        (*retQuery)[8].getString(),                              // provider type
        (*retQuery)[9].getString(),                              // provider id
        (*retQuery)[10].getString()                              // provider iCalUid
    };
}

EventsTableRow EventsTable::getByUID(const std::string &UID)
{
    auto retQuery = db->query("SELECT * FROM events WHERE uid= '%q';", UID.c_str());

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return EventsTableRow();
    }

    assert(retQuery->getRowCount() == 1);

    return EventsTableRow{
        (*retQuery)[0].getUInt32(),                              // ID
        (*retQuery)[1].getString(),                              // UID
        (*retQuery)[2].getString(),                              // title
        TimePointFromString((*retQuery)[3].getString().c_str()), // date_from
        TimePointFromString((*retQuery)[4].getString().c_str()), // date_till
        (*retQuery)[5].getUInt32(),                              // reminder
        (*retQuery)[6].getUInt32(),                              // repeat
        TimePointFromString((*retQuery)[7].getString().c_str()), // reminder_fired
        (*retQuery)[8].getString(),                              // provider type
        (*retQuery)[9].getString(),                              // provider id
        (*retQuery)[10].getString()                              // provider iCalUid
    };
}

std::vector<EventsTableRow> EventsTable::selectByDatePeriod(TimePoint date_filter,
                                                            TimePoint filter_till,
                                                            uint32_t offset,
                                                            uint32_t limit)
{
    auto retQuery = db->query("SELECT * FROM events WHERE date_from >= date('%q') AND date_from < date('%q', 'start of "
                              "day') ORDER BY datetime(date_from) LIMIT %u OFFSET %u;",
                              TimePointToString(date_filter).c_str(),
                              TimePointToString(filter_till).c_str(),
                              limit,
                              offset);

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return std::vector<EventsTableRow>();
    }

    std::vector<EventsTableRow> ret;

    do {
        ret.push_back(EventsTableRow{
            (*retQuery)[0].getUInt32(),                              // ID
            (*retQuery)[1].getString(),                              // UID
            (*retQuery)[2].getString(),                              // title
            TimePointFromString((*retQuery)[3].getString().c_str()), // date_from
            TimePointFromString((*retQuery)[4].getString().c_str()), // date_till
            (*retQuery)[5].getUInt32(),                              // reminder
            (*retQuery)[6].getUInt32(),                              // repeat
            TimePointFromString((*retQuery)[7].getString().c_str()), // reminder_fired
            (*retQuery)[8].getString(),                              // provider type
            (*retQuery)[9].getString(),                              // provider id
            (*retQuery)[10].getString()                              // provider iCalUid
        });

    } while (retQuery->nextRow());

    return ret;
}

std::vector<EventsTableRow> EventsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from events LIMIT %lu OFFSET %lu;", limit, offset);
    //"SELECT * FROM event WHERE id = "

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return std::vector<EventsTableRow>();
    }

    std::vector<EventsTableRow> ret;

    do {
        ret.push_back(EventsTableRow{
            (*retQuery)[0].getUInt32(),                              // ID
            (*retQuery)[1].getString(),                              // UID
            (*retQuery)[2].getString(),                              // title
            TimePointFromString((*retQuery)[3].getString().c_str()), // date_from
            TimePointFromString((*retQuery)[4].getString().c_str()), // date_till
            (*retQuery)[5].getUInt32(),                              // reminder
            (*retQuery)[6].getUInt32(),                              // repeat
            TimePointFromString((*retQuery)[7].getString().c_str()), // reminder_fired
            (*retQuery)[8].getString(),                              // provider type
            (*retQuery)[9].getString(),                              // provider id
            (*retQuery)[10].getString()                              // provider iCalUid

        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<EventsTableRow> EventsTable::getLimitOffsetByDate(uint32_t offset, uint32_t limit)
{

    auto retQuery = db->query("SELECT * from events ORDER BY datetime(date_from) LIMIT %u OFFSET %u;", limit, offset);

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return std::vector<EventsTableRow>();
    }

    std::vector<EventsTableRow> ret;

    do {
        ret.push_back(EventsTableRow{
            (*retQuery)[0].getUInt32(),                              // ID
            (*retQuery)[1].getString(),                              // UID
            (*retQuery)[2].getString(),                              // title
            TimePointFromString((*retQuery)[3].getString().c_str()), // date_from
            TimePointFromString((*retQuery)[4].getString().c_str()), // date_till
            (*retQuery)[5].getUInt32(),                              // reminder
            (*retQuery)[6].getUInt32(),                              // repeat
            TimePointFromString((*retQuery)[7].getString().c_str()), // reminder_fired
            (*retQuery)[8].getString(),                              // provider type
            (*retQuery)[9].getString(),                              // provider id
            (*retQuery)[10].getString()                              // provider iCalUid
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<EventsTableRow> EventsTable::getLimitOffsetByField(uint32_t offset,
                                                               uint32_t limit,
                                                               EventsTableFields field,
                                                               const char *str)
{
    std::vector<EventsTableRow> ret;

    assert(0 && "Not implemented");

    return ret;
}

bool EventsTable::drop()
{
    return db->execute("DROP TABLE events;");
}

uint32_t EventsTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM events;");

    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t EventsTable::countFromFilter(TimePoint from, TimePoint till)
{
    auto queryRet = db->query(
        "SELECT COUNT(*) FROM events WHERE date_from >= date('%q') AND date_from < date('%q', 'start of day');",
        TimePointToString(from).c_str(),
        TimePointToString(till).c_str());

    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t EventsTable::countByFieldId(const char *field, uint32_t id)
{
    assert(0 && "Not implemented");

    return 0;
}

std::vector<EventsTableRow> EventsTable::SelectFirstUpcoming(TimePoint filter_from, TimePoint filter_till)
{
    auto retQuery = db->query("SELECT DATETIME(date_from, '-' || reminder || ' minutes') AS calc_dt, * "
                              "FROM events "
                              "WHERE calc_dt >= '%q' "
                              "AND reminder_fired = '%q' "
                              "AND reminder <> -1 "
                              "ORDER BY calc_dt "
                              "LIMIT 1 ",
                              TimePointToString(filter_from).c_str(),
                              TimePointToString(TIME_POINT_INVALID).c_str());

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return std::vector<EventsTableRow>();
    }

    std::vector<EventsTableRow> ret;

    do {
        ret.push_back(EventsTableRow{
            (*retQuery)[1].getUInt32(),                              // ID
            (*retQuery)[2].getString(),                              // UID
            (*retQuery)[3].getString(),                              // title
            TimePointFromString((*retQuery)[4].getString().c_str()), // date_from
            TimePointFromString((*retQuery)[5].getString().c_str()), // date_till
            (*retQuery)[6].getUInt32(),                              // reminder
            (*retQuery)[7].getUInt32(),                              // repeat
            TimePointFromString((*retQuery)[8].getString().c_str()), // reminder_fired
            (*retQuery)[9].getString(),                              // provider type
            (*retQuery)[10].getString(),                             // provider id
            (*retQuery)[11].getString()                              // provider iCalUid
        });
    } while (retQuery->nextRow());

    return ret;
}
