// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesTable.hpp"

#include <Utils.hpp>
#include <log/log.hpp>

#include <cassert>

QuotesTable::QuotesTable(Database *db) : Table(db)
{}

QuotesTable::~QuotesTable()
{}

bool QuotesTable::create()
{
    return true;
}

bool QuotesTable::add(QuotesTableRow entry)
{
    return db->execute("INSERT or ignore INTO templates (text, lastUsageTimestamp) VALUES ('%q', '%q');",

                       entry.text.c_str(),
                       utils::to_string(entry.lastUsageTimestamp).c_str());
}

bool QuotesTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM templates where _id = %" PRIu32 ";", id);
}

bool QuotesTable::removeByField(SMSTemplateTableFields field, const char *str)
{
    assert(0 && "Not implemented");
    return false;
}

bool QuotesTable::update(QuotesTableRow entry)
{
    return db->execute("UPDATE templates SET text = '%q', lastUsageTimestamp = %q WHERE _id=%" PRIu32 ";",
                       entry.text.c_str(),
                       utils::to_string(entry.lastUsageTimestamp).c_str(),
                       entry.ID);
}

QuotesTableRow QuotesTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM templates WHERE _id = %" PRIu32 ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return QuotesTableRow();
    }

    return QuotesTableRow{
        (*retQuery)[0].getUInt32(),                      // ID
        (*retQuery)[1].getString(),                      // text
        static_cast<time_t>((*retQuery)[2].getUInt64()), // lastUsageTimestamp
    };
}

std::vector<QuotesTableRow> QuotesTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->query("SELECT * from templates ORDER BY lastUsageTimestamp DESC LIMIT %" PRIu32 " OFFSET %" PRIu32 ";",
                  limit,
                  offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<QuotesTableRow>();
    }

    std::vector<QuotesTableRow> ret;

    do {
        ret.push_back(QuotesTableRow{
            (*retQuery)[0].getUInt32(),                      // ID
            (*retQuery)[1].getString(),                      // text
            static_cast<time_t>((*retQuery)[2].getUInt64()), // lastUsageTimestamp
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<QuotesTableRow> QuotesTable::getLimitOffsetByField(uint32_t offset,
                                                               uint32_t limit,
                                                               SMSTemplateTableFields field,
                                                               const char *str)
{
    assert(0 && "Not implemented");
    return std::vector<QuotesTableRow>();
}

uint32_t QuotesTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM templates;");

    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}

uint32_t QuotesTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM templates WHERE '%q'=%" PRIu32 ";", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return (*queryRet)[0].getUInt32();
}
