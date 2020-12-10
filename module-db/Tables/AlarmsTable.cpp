// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmsTable.hpp"
#include "AlarmsRecord.hpp"

AlarmsTableRow::AlarmsTableRow(const AlarmsRecord &rec)
    : Record{rec.ID}, time{rec.time}, snooze{rec.snooze}, status{rec.status}, repeat{rec.repeat}, path{rec.path}
{}

AlarmsTableRow::AlarmsTableRow(
    uint32_t id, TimePoint time, uint32_t snooze, AlarmStatus status, uint32_t repeat, UTF8 path)
    : Record{id}, time{time}, snooze{snooze}, status{status}, repeat{repeat}, path{std::move(path)}
{}

AlarmsTableRow::AlarmsTableRow(const QueryResult &result)
    : Record{(result)[0].getUInt32()}, time{TimePointFromString((result)[1].getString().c_str())},
      snooze{(result)[2].getUInt32()}, status{static_cast<AlarmStatus>((result)[3].getInt32())},
      repeat{(result)[4].getUInt32()}, path{(result)[5].getString()}
{}

AlarmsTable::AlarmsTable(Database *db) : Table(db)
{}

bool AlarmsTable::create()
{
    return true;
}

bool AlarmsTable::add(AlarmsTableRow entry)
{
    return db->execute(
        "INSERT or ignore INTO alarms ( time, snooze, status, repeat, path ) VALUES ('%q', %lu, %i, %lu,'%q');",
        TimePointToString(entry.time).c_str(),
        entry.snooze,
        entry.status,
        entry.repeat,
        entry.path.c_str());
}

bool AlarmsTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM alarms where _id = %u;", id);
}

bool AlarmsTable::removeByField(AlarmsTableFields field, const char *str)
{
    std::string fieldName;

    switch (field) {
    case AlarmsTableFields ::Time:
        fieldName = "time";
        break;

    case AlarmsTableFields ::Snooze:
        fieldName = "snooze";
        break;

    case AlarmsTableFields ::Status:
        fieldName = "status";
        break;
    default:
        return false;
    }

    return db->execute("DELETE FROM alarms where %q = '%q';", fieldName.c_str(), str);
}

bool AlarmsTable::update(AlarmsTableRow entry)
{
    return db->execute(
        "UPDATE alarms SET time = '%q', snooze = %lu ,status = %i, repeat = %lu, path = '%q' WHERE _id=%lu;",
        TimePointToString(entry.time).c_str(),
        entry.snooze,
        entry.status,
        entry.repeat,
        entry.path.c_str(),
        entry.ID);
}

AlarmsTableRow AlarmsTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM alarms WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return AlarmsTableRow();
    }

    return AlarmsTableRow(*retQuery);
}

std::vector<AlarmsTableRow> AlarmsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from alarms ORDER BY time(time) ASC LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<AlarmsTableRow>();
    }

    std::vector<AlarmsTableRow> ret;

    do {
        ret.push_back(AlarmsTableRow(*retQuery));
    } while (retQuery->nextRow());

    return ret;
}

std::vector<AlarmsTableRow> AlarmsTable::getLimitOffsetByField(uint32_t offset,
                                                               uint32_t limit,
                                                               AlarmsTableFields field,
                                                               const char *str)
{

    std::string fieldName;
    switch (field) {
    case AlarmsTableFields::Time:
        fieldName = "time";
        break;
    case AlarmsTableFields ::Snooze:
        fieldName = "snooze";
        break;
    case AlarmsTableFields ::Status:
        fieldName = "status";
        break;
    default:
        return std::vector<AlarmsTableRow>();
    }

    auto retQuery = db->query("SELECT * from alarms WHERE %q='%q' ORDER BY time(time) LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<AlarmsTableRow>();
    }

    std::vector<AlarmsTableRow> ret;

    do {
        ret.push_back(AlarmsTableRow(*retQuery));
    } while (retQuery->nextRow());

    return ret;
}

uint32_t AlarmsTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM alarms;");
    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t AlarmsTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM alarms WHERE %q=%lu;", field, id);
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

bool AlarmsTable::updateStatuses(AlarmStatus status)
{
    return db->execute("UPDATE alarms SET status = %i;", status);
}

// mlucki
// Todo: create query that returns first upcoming alarm to invoke
//(or set of alarms that will be further processed in AlarmsTimeEvents::calcToNextEventInterval
// to programmatically choose the first upcoming one)
std::vector<AlarmsTableRow> AlarmsTable::SelectTurnedOn()
{
    auto retQuery = db->query("SELECT * from alarms WHERE status > %i;", AlarmStatus::Off);

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return std::vector<AlarmsTableRow>();
    }

    std::vector<AlarmsTableRow> ret;

    do {
        ret.push_back(AlarmsTableRow(*retQuery));
    } while (retQuery->nextRow());

    return ret;
}
