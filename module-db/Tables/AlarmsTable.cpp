// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

bool AlarmsTable::create()
{
    bool ret = true;
    ret      = db->execute(createTableQuery);

    if (!ret) {
        return false;
    }

    ret = db->execute(alarmsInitialization);
    return ret;
}

bool AlarmsTable::add(AlarmsTableRow entry)
{
    return db->execute("INSERT or ignore INTO alarms ( time, snooze, status, path ) VALUES (%lu,%lu,%lu,'%q');",
                       entry.time,
                       entry.snooze,
                       entry.status,
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
    return db->execute("UPDATE alarms SET time = %lu, snooze = %lu ,status = %lu, path = '%q' WHERE _id=%lu;",
                       entry.time,
                       entry.snooze,
                       entry.status,
                       entry.path.c_str(),
                       entry.ID);
}

AlarmsTableRow AlarmsTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM alarms WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return AlarmsTableRow();
    }

    return AlarmsTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getUInt32(), // time
        (*retQuery)[2].getUInt32(), // snooze
        (*retQuery)[3].getUInt32(), // status
        (*retQuery)[4].getString(), // path
    };
}

std::vector<AlarmsTableRow> AlarmsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from alarms ORDER BY time ASC LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<AlarmsTableRow>();
    }

    std::vector<AlarmsTableRow> ret;

    do {
        ret.push_back(AlarmsTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // time
            (*retQuery)[2].getUInt32(), // snooze
            (*retQuery)[3].getUInt32(), // status
            (*retQuery)[4].getString(), // path
        });
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

    auto retQuery = db->query("SELECT * from alarms WHERE %q='%q' ORDER BY time LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<AlarmsTableRow>();
    }

    std::vector<AlarmsTableRow> ret;

    do {
        ret.push_back(AlarmsTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // time
            (*retQuery)[2].getUInt32(), // snooze
            (*retQuery)[3].getUInt32(), // status
            (*retQuery)[4].getString(), // path
        });
    } while (retQuery->nextRow());

    return ret;
}

uint32_t AlarmsTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM alarms;");

    if (queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t AlarmsTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM alarms WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

AlarmsTableRow AlarmsTable::next(time_t time)
{
    auto retQuery = db->query("SELECT * from alarms WHERE status=1 AND time>=%u ORDER BY time ASC LIMIT 1;", time);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return AlarmsTableRow();
    }

    return AlarmsTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getUInt32(), // time
        (*retQuery)[2].getUInt32(), // snooze
        (*retQuery)[3].getUInt32(), // status
        (*retQuery)[4].getString(), // path
    };
}
