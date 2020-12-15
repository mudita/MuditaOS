// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmsStatusTable.hpp"

AlarmsStatusTable::AlarmsStatusTable(Database *db) : Table(db)
{}

bool AlarmsStatusTable::create()
{
    return true;
}

bool AlarmsStatusTable::add(AlarmsStatusRow entry)
{
    return db->execute("INSERT or ignore INTO alarmStatuses ( name ) VALUES ('%q');", entry.name.c_str());
}

uint32_t AlarmsStatusTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM alarmStatuses;");
    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

bool AlarmsStatusTable::removeById(uint32_t id)
{
    return false;
}

bool AlarmsStatusTable::update(AlarmsStatusRow entry)
{
    return false;
}

AlarmsStatusRow AlarmsStatusTable::getById(uint32_t id)
{
    return AlarmsStatusRow();
}

std::vector<AlarmsStatusRow> AlarmsStatusTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    return std::vector<AlarmsStatusRow>();
}

std::vector<AlarmsStatusRow> AlarmsStatusTable::getLimitOffsetByField(uint32_t offset,
                                                                      uint32_t limit,
                                                                      AlarmsStatusFields field,
                                                                      const char *str)
{
    return std::vector<AlarmsStatusRow>();
}

uint32_t AlarmsStatusTable::countByFieldId(const char *field, uint32_t id)
{
    return count();
}
