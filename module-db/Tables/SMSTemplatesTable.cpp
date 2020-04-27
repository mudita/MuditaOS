
#include "SMSTemplatesTable.hpp"

#include <Utils.hpp>
#include <log/log.hpp>

#include <cassert>

SMSTemplatesTable::SMSTemplatesTable(Database *db) : Table(db)
{}

SMSTemplatesTable::~SMSTemplatesTable()
{}

bool SMSTemplatesTable::Create()
{
    return db->Execute(createTableQuery);
}

bool SMSTemplatesTable::Add(SMSTemplatesTableRow entry)
{
    return db->Execute("INSERT or ignore INTO templates (text, lastUsageTimestamp) VALUES ('%q', '%q');",

                       entry.text.c_str(),
                       utils::to_string(entry.lastUsageTimestamp).c_str());
}

bool SMSTemplatesTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM templates where _id = %" PRIu32 ";", id);
}

bool SMSTemplatesTable::RemoveByField(SMSTemplatesTableFields field, const char *str)
{
    assert(0 && "Not implemented");
    return false;
}

bool SMSTemplatesTable::Update(SMSTemplatesTableRow entry)
{
    return db->Execute("UPDATE templates SET text = '%q', lastUsageTimestamp = %q WHERE _id=%" PRIu32 ";",
                       entry.text.c_str(),
                       utils::to_string(entry.lastUsageTimestamp).c_str(),
                       entry.ID);
}

SMSTemplatesTableRow SMSTemplatesTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM templates WHERE _id = %" PRIu32 ";", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return SMSTemplatesTableRow();
    }

    return SMSTemplatesTableRow{
        (*retQuery)[0].GetUInt32(),                      // ID
        (*retQuery)[1].GetString(),                      // text
        static_cast<time_t>((*retQuery)[2].GetUInt64()), // lastUsageTimestamp
    };
}

std::vector<SMSTemplatesTableRow> SMSTemplatesTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->Query("SELECT * from templates ORDER BY lastUsageTimestamp DESC LIMIT %" PRIu32 " OFFSET %" PRIu32 ";",
                  limit,
                  offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<SMSTemplatesTableRow>();
    }

    std::vector<SMSTemplatesTableRow> ret;

    do {
        ret.push_back(SMSTemplatesTableRow{
            (*retQuery)[0].GetUInt32(),                      // ID
            (*retQuery)[1].GetString(),                      // text
            static_cast<time_t>((*retQuery)[2].GetUInt64()), // lastUsageTimestamp
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<SMSTemplatesTableRow> SMSTemplatesTable::GetLimitOffsetByField(uint32_t offset,
                                                                           uint32_t limit,
                                                                           SMSTemplatesTableFields field,
                                                                           const char *str)
{
    assert(0 && "Not implemented");
    return std::vector<SMSTemplatesTableRow>();
}

uint32_t SMSTemplatesTable::GetCount()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM templates;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return (*queryRet)[0].GetUInt32();
}

uint32_t SMSTemplatesTable::GetCountByFieldID(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM templates WHERE '%q'=%" PRIu32 ";", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return (*queryRet)[0].GetUInt32();
}
