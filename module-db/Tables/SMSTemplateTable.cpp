
#include "SMSTemplateTable.hpp"

#include <Utils.hpp>
#include <log/log.hpp>

#include <cassert>

SMSTemplateTable::SMSTemplateTable(Database *db) : Table(db)
{}

SMSTemplateTable::~SMSTemplateTable()
{}

bool SMSTemplateTable::Create()
{
    return db->Execute(createTableQuery);
}

bool SMSTemplateTable::Add(SMSTemplateTableRow entry)
{
    return db->Execute("INSERT or ignore INTO templates (text, lastUsageTimestamp) VALUES ('%q', '%q');",

                       entry.text.c_str(),
                       utils::to_string(entry.lastUsageTimestamp).c_str());
}

bool SMSTemplateTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM templates where _id = %" PRIu32 ";", id);
}

bool SMSTemplateTable::RemoveByField(SMSTemplateTableFields field, const char *str)
{
    assert(0 && "Not implemented");
    return false;
}

bool SMSTemplateTable::Update(SMSTemplateTableRow entry)
{
    return db->Execute("UPDATE templates SET text = '%q', lastUsageTimestamp = %q WHERE _id=%" PRIu32 ";",
                       entry.text.c_str(),
                       utils::to_string(entry.lastUsageTimestamp).c_str(),
                       entry.ID);
}

SMSTemplateTableRow SMSTemplateTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM templates WHERE _id = %" PRIu32 ";", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return SMSTemplateTableRow();
    }

    return SMSTemplateTableRow{
        (*retQuery)[0].GetUInt32(),                      // ID
        (*retQuery)[1].GetString(),                      // text
        static_cast<time_t>((*retQuery)[2].GetUInt64()), // lastUsageTimestamp
    };
}

std::vector<SMSTemplateTableRow> SMSTemplateTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->Query("SELECT * from templates ORDER BY lastUsageTimestamp DESC LIMIT %" PRIu32 " OFFSET %" PRIu32 ";",
                  limit,
                  offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<SMSTemplateTableRow>();
    }

    std::vector<SMSTemplateTableRow> ret;

    do {
        ret.push_back(SMSTemplateTableRow{
            (*retQuery)[0].GetUInt32(),                      // ID
            (*retQuery)[1].GetString(),                      // text
            static_cast<time_t>((*retQuery)[2].GetUInt64()), // lastUsageTimestamp
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<SMSTemplateTableRow> SMSTemplateTable::GetLimitOffsetByField(uint32_t offset,
                                                                         uint32_t limit,
                                                                         SMSTemplateTableFields field,
                                                                         const char *str)
{
    assert(0 && "Not implemented");
    return std::vector<SMSTemplateTableRow>();
}

uint32_t SMSTemplateTable::GetCount()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM templates;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].GetUInt32();
}

uint32_t SMSTemplateTable::GetCountByFieldID(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM templates WHERE '%q'=%" PRIu32 ";", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return (*queryRet)[0].GetUInt32();
}
