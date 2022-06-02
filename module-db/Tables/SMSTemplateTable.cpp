// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSTemplateTable.hpp"

#include <Utils.hpp>
#include <log/log.hpp>

#include <cassert>

SMSTemplateTable::SMSTemplateTable(Database *db) : Table(db)
{}

SMSTemplateTable::~SMSTemplateTable()
{}

bool SMSTemplateTable::create()
{
    return true;
}

bool SMSTemplateTable::add(SMSTemplateTableRow entry)
{
    return db->execute("INSERT or ignore INTO templates (text, lastUsageTimestamp) VALUES ('%q', '%q');",

                       entry.text.c_str(),
                       utils::to_string(entry.lastUsageTimestamp).c_str());
}

bool SMSTemplateTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM templates where _id = %" PRIu32 ";", id);
}

bool SMSTemplateTable::removeByField(SMSTemplateTableFields field, const char *str)
{
    assert(0 && "Not implemented");
    return false;
}

bool SMSTemplateTable::update(SMSTemplateTableRow entry)
{
    return db->execute("UPDATE templates SET text = '%q', lastUsageTimestamp = %q WHERE _id=%" PRIu32 ";",
                       entry.text.c_str(),
                       utils::to_string(entry.lastUsageTimestamp).c_str(),
                       entry.ID);
}

SMSTemplateTableRow SMSTemplateTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM templates WHERE _id = %" PRIu32 ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return SMSTemplateTableRow();
    }

    return SMSTemplateTableRow{
        (*retQuery)[0].getUInt32(),                      // ID
        (*retQuery)[1].getString(),                      // text
        static_cast<time_t>((*retQuery)[2].getUInt64()), // lastUsageTimestamp
    };
}

std::vector<SMSTemplateTableRow> SMSTemplateTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->query("SELECT * from templates ORDER BY lastUsageTimestamp DESC LIMIT %" PRIu32 " OFFSET %" PRIu32 ";",
                  limit,
                  offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<SMSTemplateTableRow>();
    }

    std::vector<SMSTemplateTableRow> ret;

    do {
        ret.push_back(SMSTemplateTableRow{
            (*retQuery)[0].getUInt32(),                      // ID
            (*retQuery)[1].getString(),                      // text
            static_cast<time_t>((*retQuery)[2].getUInt64()), // lastUsageTimestamp
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<SMSTemplateTableRow> SMSTemplateTable::getLimitOffsetByField(uint32_t offset,
                                                                         uint32_t limit,
                                                                         SMSTemplateTableFields field,
                                                                         const char *str)
{
    assert(0 && "Not implemented");
    return std::vector<SMSTemplateTableRow>();
}

uint32_t SMSTemplateTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM templates;");

    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t SMSTemplateTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM templates WHERE '%q'=%" PRIu32 ";", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}
uint32_t SMSTemplateTable::getLastId()
{
    auto queryRet = db->query("SELECT MAX(_id) FROM templates;");
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}
