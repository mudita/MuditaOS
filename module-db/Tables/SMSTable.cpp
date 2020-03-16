
/*
 * @file SMSTable.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "SMSTable.hpp"
#include <log/log.hpp>

SMSTable::SMSTable(Database *db) : Table(db)
{}

SMSTable::~SMSTable()
{}

bool SMSTable::Create()
{
    return db->Execute(createTableQuery);
}

bool SMSTable::Add(SMSTableRow entry)
{
    return db->Execute("INSERT or ignore INTO sms ( thread_id,contact_id, date, date_send, error_code, body, read, "
                       "type ) VALUES (%lu,%lu,%lu,%lu,0,'%s',%d,%d);",
                       entry.threadID,
                       entry.contactID,
                       entry.date,
                       entry.dateSent,
                       entry.body.c_str(),
                       entry.isRead,
                       entry.type);
}

bool SMSTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM sms where _id = %u;", id);
}

bool SMSTable::RemoveByField(SMSTableFields field, const char *str)
{
    std::string fieldName;

    switch (field) {
    case SMSTableFields ::ThreadID:
        fieldName = "thread_id";
        break;

    case SMSTableFields ::ContactID:
        fieldName = "contact_id";
        break;

    case SMSTableFields ::Date:
        fieldName = "date";
        break;
    default:
        return false;
    }

    return db->Execute("DELETE FROM sms where %s = '%s';", fieldName.c_str(), str);
}

bool SMSTable::Update(SMSTableRow entry)
{
    return db->Execute("UPDATE sms SET thread_id = %lu, contact_id = %lu ,date = %lu, date_send = %lu, error_code = 0, "
                       "body = '%s', read = %d, type =%d WHERE _id=%lu;",
                       entry.threadID,
                       entry.contactID,
                       entry.date,
                       entry.dateSent,
                       entry.body.c_str(),
                       entry.isRead,
                       entry.type,
                       entry.ID);
}

SMSTableRow SMSTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM sms WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return SMSTableRow();
    }

    return SMSTableRow{
        (*retQuery)[0].GetUInt32(),                       // ID
        (*retQuery)[1].GetUInt32(),                       // threadID
        (*retQuery)[2].GetUInt32(),                       // contactID
        (*retQuery)[3].GetUInt32(),                       // date
        (*retQuery)[4].GetUInt32(),                       // dateSent
        (*retQuery)[5].GetUInt32(),                       // errorCode
        (*retQuery)[6].GetString(),                       // body
        (*retQuery)[7].GetBool(),                         // isRead
        static_cast<SMSType>((*retQuery)[8].GetUInt32()), // type
    };
}

std::vector<SMSTableRow> SMSTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->Query("SELECT * from sms ORDER BY date DESC LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{
            (*retQuery)[0].GetUInt32(),                       // ID
            (*retQuery)[1].GetUInt32(),                       // threadID
            (*retQuery)[2].GetUInt32(),                       // contactID
            (*retQuery)[3].GetUInt32(),                       // date
            (*retQuery)[4].GetUInt32(),                       // dateSent
            (*retQuery)[5].GetUInt32(),                       // errorCode
            (*retQuery)[6].GetString(),                       // body
            (*retQuery)[7].GetBool(),                         // isRead
            static_cast<SMSType>((*retQuery)[8].GetUInt32()), // type
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<SMSTableRow> SMSTable::GetLimitOffsetByField(uint32_t offset,
                                                         uint32_t limit,
                                                         SMSTableFields field,
                                                         const char *str)
{

    std::string fieldName;
    switch (field) {
    case SMSTableFields::Date:
        fieldName = "date";
        break;
    case SMSTableFields ::ContactID:
        fieldName = "contact_id";
        break;
    case SMSTableFields ::ThreadID:
        fieldName = "thread_id";
        break;
    default:
        return std::vector<SMSTableRow>();
    }

    auto retQuery = db->Query("SELECT * from sms WHERE %s='%s' ORDER BY date DESC LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{
            (*retQuery)[0].GetUInt32(),                       // ID
            (*retQuery)[1].GetUInt32(),                       // threadID
            (*retQuery)[2].GetUInt32(),                       // contactID
            (*retQuery)[3].GetUInt32(),                       // date
            (*retQuery)[4].GetUInt32(),                       // dateSent
            (*retQuery)[5].GetUInt32(),                       // errorCode
            (*retQuery)[6].GetString(),                       // body
            (*retQuery)[7].GetBool(),                         // isRead
            static_cast<SMSType>((*retQuery)[8].GetUInt32()), // type
        });
    } while (retQuery->NextRow());

    return ret;
}
uint32_t SMSTable::GetCount(EntryState state)
{
    std::string query = "SELECT COUNT(*) FROM sms ";
    switch (state) {
    case EntryState::ALL:
        break;
    case EntryState::READ:
        query += "WHERE sms.read=1";
        break;
    case EntryState::UNREAD:
        query += "WHERE sms.read=0";
        break;
    };
    query += ";";

    auto queryRet = db->Query(query.c_str());
    if (queryRet == nullptr || queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t SMSTable::GetCount()
{
    return GetCount(EntryState::ALL);
}

uint32_t SMSTable::GetCountByFieldID(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM sms WHERE %s=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}
