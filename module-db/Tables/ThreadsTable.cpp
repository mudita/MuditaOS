
/*
 * @file ThreadsTable.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "ThreadsTable.hpp"
#include "log/log.hpp"

ThreadsTable::ThreadsTable(Database *db) : Table(db)
{}

ThreadsTable::~ThreadsTable()
{}

bool ThreadsTable::Create()
{
    // Create necessary tables
    if (!db->Execute(createTableQuery))
        return false;
    if (!db->Execute(createTableThreadsCounterQuery))
        return false;
    if (!db->Execute(threadsCounterInsertionQuery))
        return false;
    if (!db->Execute(threadInsertTriggerQuery))
        return false;
    if (!db->Execute(threadRemoveTriggerQuery))
        return false;

    return true;
}

bool ThreadsTable::Add(ThreadsTableRow entry)
{

    return db->Execute("INSERT or ignore INTO threads ( date, msg_count, read, contact_id, snippet, last_dir ) VALUES "
                       "( %lu, 0, 0, %lu, '%q', %lu );",
                       entry.date,
                       entry.contactID,
                       entry.snippet.c_str(),
                       entry.type);
}

bool ThreadsTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM threads where _id = %u;", id);
}

bool ThreadsTable::Update(ThreadsTableRow entry)
{
    return db->Execute("UPDATE threads SET date = %lu, msg_count = %lu ,read = %lu, contact_id = %lu, snippet = '%q', "
                       "last_dir = %lu WHERE _id=%lu;",
                       entry.date,
                       entry.msgCount,
                       entry.msgRead,
                       entry.contactID,
                       entry.snippet.c_str(),
                       entry.type,
                       entry.ID);
}

ThreadsTableRow ThreadsTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM threads WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return ThreadsTableRow();
    }

    return ThreadsTableRow{
        (*retQuery)[0].GetUInt32(),                       // ID
        (*retQuery)[1].GetUInt32(),                       // date
        (*retQuery)[2].GetUInt32(),                       // msgCount
        (*retQuery)[3].GetUInt32(),                       // msgRead
        (*retQuery)[4].GetUInt32(),                       // contactID
        (*retQuery)[5].GetString(),                       // snippet
        static_cast<SMSType>((*retQuery)[6].GetUInt32()), // type/last-dir
    };
}

void fillRetQuery(std::vector<ThreadsTableRow> &ret, const std::unique_ptr<QueryResult> &retQuery)
{
    do {
        ret.push_back(ThreadsTableRow{
            (*retQuery)[0].GetUInt32(),                       // ID
            (*retQuery)[1].GetUInt32(),                       // date
            (*retQuery)[2].GetUInt32(),                       // msgCount
            (*retQuery)[3].GetUInt32(),                       // msgRead
            (*retQuery)[4].GetUInt32(),                       // contactID
            (*retQuery)[5].GetString(),                       // snippet
            static_cast<SMSType>((*retQuery)[6].GetUInt32()), // type/last-dir
        });
    } while (retQuery->NextRow());
}

std::vector<ThreadsTableRow> ThreadsTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{

    auto retQuery = db->Query("SELECT * from threads ORDER BY date DESC LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ThreadsTableRow>();
    }

    std::vector<ThreadsTableRow> ret;
    fillRetQuery(ret, retQuery);
    return ret;
}

std::vector<ThreadsTableRow> ThreadsTable::GetLimitOffsetByField(uint32_t offset,
                                                                 uint32_t limit,
                                                                 ThreadsTableFields field,
                                                                 const char *str)
{

    std::string fieldName;
    switch (field) {
    case ThreadsTableFields::Date:
        fieldName = "date";
        break;
    case ThreadsTableFields::MsgCount:
        fieldName = "msg_count";
        break;
    case ThreadsTableFields ::ContactID:
        fieldName = "contact_id";
        break;
    default:
        return std::vector<ThreadsTableRow>();
    }

    std::string querry = "SELECT * from threads WHERE " + fieldName + " = '" + str + "' ORDER BY date";
    if (limit != 0) {
        querry += " LIMIT " + std::to_string(limit);
    }
    if (offset != 0) {
        querry += " OFFSET " + std::to_string(offset);
    }
    querry += ";";

    auto retQuery = db->Query(querry.c_str());

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ThreadsTableRow>();
    }

    std::vector<ThreadsTableRow> ret;
    fillRetQuery(ret, retQuery);
    return ret;
}

uint32_t ThreadsTable::GetCount()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM threads;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ThreadsTable::GetCountByFieldID(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM threads WHERE %q=%u;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

std::pair<uint32_t, std::vector<ThreadsTableRow>> ThreadsTable::getBySMSQuery(std::string text,
                                                                              uint32_t offset,
                                                                              uint32_t limit)
{
    auto ret       = std::pair<uint32_t, std::vector<ThreadsTableRow>>{0, {}};
    auto count_ret = db->Query("SELECT COUNT (*) from sms WHERE sms.body like \"%%%q%%\"", text.c_str());
    ret.first      = count_ret == nullptr ? 0 : (*count_ret)[0].GetUInt32();
    if (ret.first != 0) {
        auto retQuery =
            db->Query("SELECT * from sms WHERE sms.body like \"%%%q%%\" ORDER BY date DESC LIMIT %lu OFFSET %lu;",
                      text.c_str(),
                      limit,
                      offset);
        do {
            ret.second.push_back(ThreadsTableRow{
                .ID        = (*retQuery)[0].GetUInt32(),
                .date      = (*retQuery)[3].GetUInt32(),
                .msgCount  = 0,
                .msgRead   = (*retQuery)[7].GetBool(),
                .contactID = (*retQuery)[2].GetUInt32(),
                .snippet   = (*retQuery)[6].GetString(),
                .type      = static_cast<SMSType>((*retQuery)[8].GetUInt32()),
            });
        } while (retQuery->NextRow());
    }
    else {
        ret.second = {};
    }
    return ret;
}
