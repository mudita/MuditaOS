// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmEventsTable.hpp"

#include <Interface/AlarmEventRecord.hpp>

AlarmEventsTableRow::AlarmEventsTableRow(uint32_t id,
                                         const UTF8 &name,
                                         TimePoint startDate,
                                         TimePoint endDate,
                                         uint32_t duration,
                                         bool isAllDay,
                                         const std::string &rruleText,
                                         const std::string &musicTone,
                                         bool enabled,
                                         uint32_t snoozeDuration)
    : EventsTableRow{id, name, startDate, endDate, duration, isAllDay, rruleText}, musicTone{musicTone},
      enabled{enabled}, snoozeDuration{snoozeDuration}
{}

AlarmEventsTableRow::AlarmEventsTableRow(const AlarmEventRecord &rec)
    : AlarmEventsTableRow{rec.ID,
                          rec.name,
                          rec.startDate,
                          rec.endDate,
                          rec.duration,
                          rec.isAllDay,
                          rec.rruleText,
                          rec.musicTone,
                          rec.enabled,
                          rec.snoozeDuration}
{}

AlarmEventsTableRow::AlarmEventsTableRow(const QueryResult &result)
    : EventsTableRow{result[0].getUInt32(),
                     result[1].getString().c_str(),
                     TimePointFromString(result[2].getString().c_str()),
                     TimePointFromString(result[3].getString().c_str()),
                     result[4].getUInt32(),
                     result[5].getBool(),
                     result[6].getString().c_str()},
      musicTone{result[9].getString()}, enabled{result[10].getBool()}, snoozeDuration{result[11].getUInt32()}
{}

auto AlarmEventsTableRow::isValid() const -> bool
{
    return (startDate != TIME_POINT_INVALID && ID != 0);
}

AlarmEventsTable::AlarmEventsTable(Database *db) : Table(db)
{
    createTableRow = [](const QueryResult &retQuery) { return AlarmEventsTableRow(retQuery); };
}

bool AlarmEventsTable::create()
{
    return true;
}

bool AlarmEventsTable::add(AlarmEventsTableRow entry)
{
    auto ret = db->execute("BEGIN TRANSACTION;"
                           "INSERT or ignore INTO events ( name, start_date, end_date, duration, is_all_day, rrule)"
                           "VALUES ('%q', '%q', '%q', %lu, %d, '%q');",
                           entry.name.c_str(),
                           TimePointToString(entry.startDate).c_str(),
                           TimePointToString(entry.endDate).c_str(),
                           entry.duration,
                           entry.isAllDay,
                           entry.rruleText.c_str());
    if (!ret) {
        return false;
    }
    auto eventId = db->getLastInsertRowId();
    if (eventId == 0) {
        return false;
    }
    return db->execute("INSERT or ignore INTO alarm_events ( event_id, music_tone, enabled, snooze_duration)"
                       "VALUES ('%lu', '%q', '%d', %lu);"
                       "COMMIT;",
                       eventId,
                       entry.musicTone.c_str(),
                       entry.enabled,
                       entry.snoozeDuration);
}

bool AlarmEventsTable::removeById(uint32_t id)
{
    return db->execute("BEGIN TRANSACTION; "
                       "DELETE FROM events "
                       "WHERE events._id = (SELECT event_id FROM alarm_events WHERE _id=%lu); "
                       "DELETE FROM alarm_events "
                       "WHERE event_id = %lu; "
                       "COMMIT;",
                       id,
                       id);
}

bool AlarmEventsTable::update(AlarmEventsTableRow entry)
{
    return db->execute(
        "BEGIN TRANSACTION; "
        "UPDATE events SET name = '%q', start_date = '%q', end_date = '%q', duration = %d, is_all_day = %d, "
        "rrule = '%q' WHERE _id= (SELECT event_id FROM alarm_events WHERE _id=%lu); "
        "UPDATE alarm_events SET music_tone = '%q', enabled = '%d', snooze_duration = '%d' "
        "WHERE _id=%lu; "
        "COMMIT;",
        entry.name.c_str(),
        TimePointToString(entry.startDate).c_str(),
        TimePointToString(entry.endDate).c_str(),
        entry.duration,
        entry.isAllDay,
        entry.rruleText.c_str(),
        entry.ID,
        entry.musicTone.c_str(),
        entry.enabled,
        entry.snoozeDuration,
        entry.ID);
}

AlarmEventsTableRow AlarmEventsTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * "
                              "FROM events as e, alarm_events as ae "
                              "WHERE e._id=ae.event_id AND e._id = %u",
                              id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return AlarmEventsTableRow();
    }

    return AlarmEventsTableRow(*retQuery);
}

std::vector<AlarmEventsTableRow> AlarmEventsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * FROM events e "
                              "JOIN alarm_events ae ON ae.event_id = e._id "
                              "LIMIT %lu OFFSET %lu ",
                              limit,
                              offset);

    return retQueryUnpack(std::move(retQuery));
}

std::vector<AlarmEventsTableRow> AlarmEventsTable::getLimitOffsetByField(uint32_t offset,
                                                                         uint32_t limit,
                                                                         AlarmEventsTableFields field,
                                                                         const char *str)
{
    std::unique_ptr<QueryResult> retQuery = nullptr;
    const auto &fieldName                 = getFieldName(field);

    if (fieldName.empty()) {
        return {};
    }

    retQuery = db->query("SELECT * FROM events e "
                         "JOIN alarm_events ae ON ae.event_id = e._id "
                         "WHERE %q = '%q' "
                         "ORDER BY start_date LIMIT %lu OFFSET %lu;",
                         fieldName.c_str(),
                         str,
                         limit,
                         offset);

    return retQueryUnpack(std::move(retQuery));
}

std::pair<std::vector<AlarmEventsTableRow>, std::uint32_t> AlarmEventsTable::getBetweenDates(TimePoint startDate,
                                                                                             TimePoint endDate,
                                                                                             std::uint32_t offset,
                                                                                             std::uint32_t limit)
{
    auto retQuery = db->query("SELECT * FROM events e "
                              "JOIN alarm_events ae ON ae.event_id = e._id "
                              "WHERE start_date BETWEEN '%q' and '%q'"
                              "ORDER BY start_date LIMIT %lu OFFSET %lu;",
                              TimePointToString(startDate).c_str(),
                              TimePointToString(endDate).c_str(),
                              limit,
                              offset);

    return {retQueryUnpack(std::move(retQuery)), count()};
}

std::vector<AlarmEventsTableRow> AlarmEventsTable::getRecurringBetweenDates(TimePoint startDate,
                                                                            TimePoint endDate,
                                                                            uint32_t offset,
                                                                            uint32_t limit)
{
    auto retQuery = db->query("SELECT * FROM events e "
                              "JOIN alarm_events ae ON ae.event_id = e._id "
                              "WHERE(start_date <= '%q' AND end_date >= '%q') "
                              "AND rrule <> '' "
                              "ORDER BY start_date LIMIT %lu OFFSET %lu;",
                              TimePointToString(endDate).c_str(),
                              TimePointToString(startDate).c_str(),
                              limit,
                              offset);
    return retQueryUnpack(std::move(retQuery));
}

std::vector<AlarmEventsTableRow> AlarmEventsTable::getNext(TimePoint start, uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * FROM events e "
                              "JOIN alarm_events ae ON ae.event_id = e._id "
                              "WHERE start_date = "
                              "	("
                              "		SELECT MIN(e.start_date) FROM events as e "
                              "		WHERE e.start_date > '%q'"
                              "	) "
                              "LIMIT %lu OFFSET %lu;",
                              TimePointToString(start).c_str(),
                              limit,
                              offset);

    return retQueryUnpack(std::move(retQuery));
}

uint32_t AlarmEventsTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM alarm_events;");
    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t AlarmEventsTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM alarm_events "
                              "JOIN events ON alarm_events.event_id = events._id "
                              "WHERE %q=%lu;",
                              field,
                              id);
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

std::string AlarmEventsTable::getFieldName(AlarmEventsTableFields field)
{
    switch (field) {
    case AlarmEventsTableFields::Name:
        return "name";
    case AlarmEventsTableFields::StartDate:
        return "start_date";
    case AlarmEventsTableFields::EndDate:
        return "end_date";
    case AlarmEventsTableFields::Duration:
        return "duration";
    case AlarmEventsTableFields::IsAllDay:
        return "is_all_day";
    case AlarmEventsTableFields::Rrule:
        return "rrule";
    case AlarmEventsTableFields::MusicTone:
        return "music_tone";
    case AlarmEventsTableFields::Enabled:
        return "enabled";
    case AlarmEventsTableFields::SnoozeDuration:
        return "snooze_duration";
    default:
        return "";
    }
}
