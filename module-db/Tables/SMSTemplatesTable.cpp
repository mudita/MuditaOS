
#include "SMSTemplatesTable.hpp"
#include <log/log.hpp>

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
    return db->Execute(
        "INSERT or ignore INTO templates (text, usageTimestamp) VALUES (%s,%lu);", // TODO: change to 64 bit value
        entry.text.c_str(),
        entry.usageTimestamp);
}

bool SMSTemplatesTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM templates where _id = %u;", id);
}

bool SMSTemplatesTable::RemoveByField(SMSTemplatesTableFields field, const char *str)
{
    LOG_ERROR("RemoveByField not implemented");
    return false;
}

bool SMSTemplatesTable::Update(SMSTemplatesTableRow entry)
{
    return db->Execute("UPDATE templates SET text = %s, usageTimestamp = %lu WHERE _id=%lu;",
                       entry.text.c_str(),
                       entry.usageTimestamp,
                       entry.ID);
}

SMSTemplatesTableRow SMSTemplatesTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM templates WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return SMSTemplatesTableRow();
    }

    return SMSTemplatesTableRow{
        (*retQuery)[0].GetUInt32(), // ID
        (*retQuery)[1].GetString(), // text
        (*retQuery)[2].GetUInt32(), // usageTimestamp
    };
}

std::vector<SMSTemplatesTableRow> SMSTemplatesTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->Query("SELECT * from templates ORDER BY usageTimestamp DESC LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<SMSTemplatesTableRow>();
    }

    std::vector<SMSTemplatesTableRow> ret;

    do {
        ret.push_back(SMSTemplatesTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetString(), // text
            (*retQuery)[2].GetUInt32(), // usageTimestamp
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<SMSTemplatesTableRow> SMSTemplatesTable::GetLimitOffsetByField(uint32_t offset,
                                                                           uint32_t limit,
                                                                           SMSTemplatesTableFields field,
                                                                           const char *str)
{
    LOG_ERROR("Not implemented");
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
    auto queryRet = db->Query("SELECT COUNT(*) FROM templates WHERE %s=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}
