// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmEventsTable.hpp"
#include "Common/Types.hpp"

#include <Interface/AlarmEventRecord.hpp>

AlarmEventsTableRow::AlarmEventsTableRow(uint32_t id,
                                         uint32_t hourOfDay,
                                         uint32_t minuteOfHour,
                                         const std::string &musicTone,
                                         bool enabled,
                                         uint32_t snoozeDuration,
                                         const std::string &rruleText)
    : Record{id}, hourOfDay{hourOfDay}, minuteOfHour{minuteOfHour}, musicTone{musicTone}, enabled{enabled},
      snoozeDuration{snoozeDuration}, rruleText{rruleText}
{}

AlarmEventsTableRow::AlarmEventsTableRow(const AlarmEventRecord &rec)
    : AlarmEventsTableRow{rec.ID,
                          static_cast<uint32_t>(rec.alarmTime.hourOfDay.count()),
                          static_cast<uint32_t>(rec.alarmTime.minuteOfHour.count()),
                          rec.musicTone,
                          rec.enabled,
                          rec.snoozeDuration,
                          rec.rruleText}
{}

AlarmEventsTableRow::AlarmEventsTableRow(const QueryResult &result)
    : Record{result[0].getUInt32()}, hourOfDay{result[1].getUInt32()},
      minuteOfHour{result[2].getUInt32()}, musicTone{result[3].getString()}, enabled{result[4].getBool()},
      snoozeDuration{result[5].getUInt32()}, rruleText{result[6].getString()}
{}

auto AlarmEventsTableRow::isValid() const -> bool
{
    return (ID != 0);
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
    return db->execute("INSERT or ignore INTO alarms ( hour, minute, music_tone, enabled, snooze_duration, rrule)"
                       "VALUES (" u32_c u32_c str_c u32_c u32_c str_ ");",
                       entry.hourOfDay,
                       entry.minuteOfHour,
                       entry.musicTone.c_str(),
                       entry.enabled,
                       entry.snoozeDuration,
                       entry.rruleText.c_str());
}

bool AlarmEventsTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM alarms WHERE alarms._id=" u32_ ";", id);
}

bool AlarmEventsTable::update(AlarmEventsTableRow entry)
{
    return db->execute("UPDATE alarms SET hour=" u32_c "minute=" u32_c "music_tone=" str_c "enabled=" u32_c
                       "snooze_duration=" u32_c "rrule=" str_ " WHERE _id=" u32_ ";",
                       entry.hourOfDay,
                       entry.minuteOfHour,
                       entry.musicTone.c_str(),
                       entry.enabled,
                       entry.snoozeDuration,
                       entry.rruleText.c_str(),
                       entry.ID);
}

AlarmEventsTableRow AlarmEventsTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM alarms WHERE _id=" u32_ ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return AlarmEventsTableRow();
    }

    return AlarmEventsTableRow(*retQuery);
}

std::vector<AlarmEventsTableRow> AlarmEventsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->query("SELECT * FROM alarms ORDER BY hour, minute LIMIT " u32_ " OFFSET " u32_ ";", limit, offset);

    return retQueryUnpack(std::move(retQuery));
}

std::vector<AlarmEventsTableRow> AlarmEventsTable::getEnabled()
{
    auto retQuery = db->query("SELECT * FROM alarms WHERE enabled = 1;");

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

    retQuery =
        db->query("SELECT * FROM alarms e WHERE %q=" str_ "ORDER BY hour, minute LIMIT " u32_ " OFFSET " u32_c ";",
                  fieldName.c_str(),
                  str,
                  limit,
                  offset);

    return retQueryUnpack(std::move(retQuery));
}

auto AlarmEventsTable::toggleAll(bool toggle) -> bool
{
    auto ret = db->execute("UPDATE alarms SET enabled=" u32_ ";", static_cast<int>(toggle));

    return ret;
}

uint32_t AlarmEventsTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM alarms;");
    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t AlarmEventsTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM alarms WHERE %q=" u32_ ";", field, id);
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

std::string AlarmEventsTable::getFieldName(AlarmEventsTableFields field)
{
    switch (field) {
    case AlarmEventsTableFields::AlarmTimeHour:
        return "hour";
    case AlarmEventsTableFields::AlarmTimeMinute:
        return "minute";
    case AlarmEventsTableFields::MusicTone:
        return "music_tone";
    case AlarmEventsTableFields::Enabled:
        return "enabled";
    case AlarmEventsTableFields::SnoozeDuration:
        return "snooze_duration";
    case AlarmEventsTableFields::Rrule:
        return "rrule";
    default:
        return "";
    }
}
