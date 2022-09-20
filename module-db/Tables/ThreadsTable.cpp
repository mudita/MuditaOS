// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ThreadsTable.hpp"
#include "Common/Types.hpp"
#include <log/log.hpp>

ThreadsTable::ThreadsTable(Database *db) : Table(db)
{}

bool ThreadsTable::create()
{
    return true;
}

bool ThreadsTable::add(ThreadsTableRow entry)
{

    return db->execute(
        "INSERT or ignore INTO threads ( date, msg_count, read, contact_id, number_id, snippet, last_dir ) VALUES "
        "(" u32_c u32_c u32_c u32_c u32_c str_c u32_ ");",
        entry.date,
        entry.msgCount,
        entry.unreadMsgCount,
        entry.contactID,
        entry.numberID,
        entry.snippet.c_str(),
        entry.type);
}

bool ThreadsTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM threads where _id= " u32_ ";", id);
}

bool ThreadsTable::update(ThreadsTableRow entry)
{
    return db->execute("UPDATE threads SET date=" u32_c "msg_count=" u32_c "read=" u32_c "contact_id=" u32_c
                       "number_id=" u32_c "snippet=" str_c "last_dir=" u32_ " WHERE _id=" u32_ ";",
                       entry.date,
                       entry.msgCount,
                       entry.unreadMsgCount,
                       entry.contactID,
                       entry.numberID,
                       entry.snippet.c_str(),
                       entry.type,
                       entry.ID);
}

ThreadsTableRow ThreadsTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM threads WHERE _id=" u32_ ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return ThreadsTableRow();
    }

    return ThreadsTableRow{
        (*retQuery)[0].getUInt32(),                       // ID
        (*retQuery)[1].getUInt32(),                       // date
        (*retQuery)[2].getUInt32(),                       // msgCount
        (*retQuery)[3].getUInt32(),                       // unreadMsgCount
        (*retQuery)[4].getUInt32(),                       // contactID
        (*retQuery)[5].getUInt32(),                       // numberID
        (*retQuery)[6].getString(),                       // snippet
        static_cast<SMSType>((*retQuery)[7].getUInt32()), // type/last-dir
    };
}

void fillRetQuery(std::vector<ThreadsTableRow> &ret, const std::unique_ptr<QueryResult> &retQuery)
{
    do {
        ret.push_back(ThreadsTableRow{
            (*retQuery)[0].getUInt32(),                       // ID
            (*retQuery)[1].getUInt32(),                       // date
            (*retQuery)[2].getUInt32(),                       // msgCount
            (*retQuery)[3].getUInt32(),                       // unreadMsgCount
            (*retQuery)[4].getUInt32(),                       // contactID
            (*retQuery)[5].getUInt32(),                       // numberID
            (*retQuery)[6].getString(),                       // snippet
            static_cast<SMSType>((*retQuery)[7].getUInt32()), // type/last-dir
        });
    } while (retQuery->nextRow());
}

std::vector<ThreadsTableRow> ThreadsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{

    auto retQuery =
        db->query("SELECT * from threads ORDER BY date DESC LIMIT " u32_ " OFFSET " u32_ ";", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ThreadsTableRow>();
    }

    std::vector<ThreadsTableRow> ret;
    fillRetQuery(ret, retQuery);
    return ret;
}

std::vector<ThreadsTableRow> ThreadsTable::getLimitOffsetByField(uint32_t offset,
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
    case ThreadsTableFields::ContactID:
        fieldName = "contact_id";
        break;
    case ThreadsTableFields::NumberID:
        fieldName = "number_id";
        break;
    default:
        return std::vector<ThreadsTableRow>();
    }

    std::string query = "SELECT * from threads WHERE " + fieldName + " = '" + str + "' ORDER BY date";
    if (limit != 0) {
        query += " LIMIT " + std::to_string(limit);
    }
    if (offset != 0) {
        query += " OFFSET " + std::to_string(offset);
    }
    query += ";";

    auto retQuery = db->query(query.c_str());

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ThreadsTableRow>();
    }

    std::vector<ThreadsTableRow> ret;
    fillRetQuery(ret, retQuery);
    return ret;
}

uint32_t ThreadsTable::count()
{
    return count(EntryState::ALL);
}

uint32_t ThreadsTable::count(EntryState state)
{
    std::string query = "SELECT COUNT(*) FROM threads ";
    switch (state) {
    case EntryState::ALL:
        break;
    case EntryState::READ:
        query += "WHERE threads.read=0";
        break;
    case EntryState::UNREAD:
        query += "WHERE threads.read>0";
        break;
    };
    query += ";";

    auto queryRet = db->query(query.c_str());
    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t ThreadsTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM threads WHERE %q=" u32_ ";", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

std::pair<uint32_t, std::vector<ThreadsTableRow>> ThreadsTable::getBySMSQuery(std::string text,
                                                                              uint32_t offset,
                                                                              uint32_t limit)
{
    const auto totalCountQuery =
        db->query("SELECT COUNT(*) from sms INNER JOIN threads ON sms.thread_id=threads._id where sms.body "
                  "like \'%%%q%%\'",
                  text.c_str());

    if ((totalCountQuery == nullptr) || (totalCountQuery->getRowCount() == 0)) {
        return {};
    }

    const auto retQuery = db->query("SELECT * from sms INNER JOIN threads ON sms.thread_id=threads._id where sms.body "
                                    "like \'%%%q%%\' ORDER BY date DESC LIMIT " u32_ " OFFSET " u32_ ";",
                                    text.c_str(),
                                    limit,
                                    offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return {};
    }

    auto ret = std::pair<uint32_t, std::vector<ThreadsTableRow>>{(*totalCountQuery)[0].getUInt32(), {}};
    fillRetQuery(ret.second, retQuery);
    return ret;
}
