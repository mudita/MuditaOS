/*
 * AlarmsTable.cpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#include "AlarmsTable.hpp"

AlarmsTable::AlarmsTable(Database *db) : Table(db)
{}

AlarmsTable::~AlarmsTable()
{}

bool AlarmsTable::Create()
{
    bool ret = true;
    ret      = db->Execute(createTableQuery);

    if (!ret) {
        return false;
    }

    ret = db->Execute(alarmsInitialization);
    return ret;
}

bool AlarmsTable::Add(AlarmsTableRow entry)
{
    return db->Execute("INSERT or ignore INTO alarms ( time, snooze, status, path ) VALUES (%lu,%lu,%lu,'%q');",
                       entry.time,
                       entry.snooze,
                       entry.status,
                       entry.path.c_str());
}

bool AlarmsTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM alarms where _id = %u;", id);
}

bool AlarmsTable::RemoveByField(AlarmsTableFields field, const char *str)
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

    return db->Execute("DELETE FROM alarms where %q = '%q';", fieldName.c_str(), str);
}

bool AlarmsTable::Update(AlarmsTableRow entry)
{
    return db->Execute("UPDATE alarms SET time = %lu, snooze = %lu ,status = %lu, path = '%q' WHERE _id=%lu;",
                       entry.time,
                       entry.snooze,
                       entry.status,
                       entry.path.c_str(),
                       entry.ID);
}

AlarmsTableRow AlarmsTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM alarms WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return AlarmsTableRow();
    }

    return AlarmsTableRow{
        (*retQuery)[0].GetUInt32(), // ID
        (*retQuery)[1].GetUInt32(), // time
        (*retQuery)[2].GetUInt32(), // snooze
        (*retQuery)[3].GetUInt32(), // status
        (*retQuery)[4].GetString(), // path
    };
}

std::vector<AlarmsTableRow> AlarmsTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->Query("SELECT * from alarms ORDER BY time ASC LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<AlarmsTableRow>();
    }

    std::vector<AlarmsTableRow> ret;

    do {
        ret.push_back(AlarmsTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // time
            (*retQuery)[2].GetUInt32(), // snooze
            (*retQuery)[3].GetUInt32(), // status
            (*retQuery)[4].GetString(), // path
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<AlarmsTableRow> AlarmsTable::GetLimitOffsetByField(uint32_t offset,
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

    auto retQuery = db->Query("SELECT * from alarms WHERE %q='%q' ORDER BY time LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<AlarmsTableRow>();
    }

    std::vector<AlarmsTableRow> ret;

    do {
        ret.push_back(AlarmsTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // time
            (*retQuery)[2].GetUInt32(), // snooze
            (*retQuery)[3].GetUInt32(), // status
            (*retQuery)[4].GetString(), // path
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t AlarmsTable::GetCount()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM alarms;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t AlarmsTable::GetCountByFieldID(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM alarms WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

AlarmsTableRow AlarmsTable::GetNext(time_t time)
{
    auto retQuery = db->Query("SELECT * from alarms WHERE status=1 AND time>=%u ORDER BY time ASC LIMIT 1;", time);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return AlarmsTableRow();
    }

    return AlarmsTableRow{
        (*retQuery)[0].GetUInt32(), // ID
        (*retQuery)[1].GetUInt32(), // time
        (*retQuery)[2].GetUInt32(), // snooze
        (*retQuery)[3].GetUInt32(), // status
        (*retQuery)[4].GetString(), // path
    };
}
