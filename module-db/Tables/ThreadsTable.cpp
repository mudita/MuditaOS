
/*
 * @file ThreadsTable.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ThreadsTable.hpp"


ThreadsTable::ThreadsTable(Database* db):db(db){
}

ThreadsTable::~ThreadsTable() {

}

bool ThreadsTable::Create() {
    // Create necessary tables
    if(db->Execute(createTableQuery) == false) return false;
    if(db->Execute(createTableThreadsCounterQuery)) return false;
    if(db->Execute(threadsCounterInsertionQuery)) return false;
    if(db->Execute(threadInsertTriggerQuery)) return false;
    if(db->Execute(threadRemoveTriggerQuery)) return false;

    return true;
}

bool ThreadsTable::Add(ThreadsTableRow entry) {

    return db->Execute(
            "INSERT INTO threads ( date, msg_count, read, contact_id, snippet, last_dir ) VALUES ( %lu, 0, 0, %lu, '%s', %lu );",
            entry.date,
            entry.contactID,
            entry.snippet.c_str(),
            entry.type
    );
}

bool ThreadsTable::RemoveByID(uint32_t id) {
    return db->Execute("DELETE FROM threads where _id = %u;", id);
}

bool ThreadsTable::Update(ThreadsTableRow entry) {
    return db->Execute(
            "UPDATE threads SET date = %lu, msg_count = %lu ,read = %lu, contact_id = %lu, snippet = '%s', last_dir = %lu WHERE _id=%lu;",
            entry.date,
            entry.msgCount,
            entry.msgRead,
            entry.contactID,
            entry.snippet.c_str(),
            entry.type,
            entry.ID
    );
}

ThreadsTableRow ThreadsTable::GetByID(uint32_t id) {
    auto retQuery = db->Query("SELECT * FROM threads WHERE _id= %u;", id);

    if ( (retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return ThreadsTableRow();
    }

    return ThreadsTableRow{(*retQuery)[0].GetUInt32(),  // ID
                           (*retQuery)[1].GetUInt32(),    // date
                           (*retQuery)[2].GetUInt32(),    // msgCount
                           (*retQuery)[3].GetUInt32(),    // msgRead
                           (*retQuery)[4].GetUInt32(),    // contactID
                           (*retQuery)[5].GetString(),    // snippet
                           static_cast<SMSType >((*retQuery)[6].GetUInt32()),    // type/last-dir
    };
}

std::vector<ThreadsTableRow> ThreadsTable::GetLimitOffset(uint32_t offset, uint32_t limit) {

    auto retQuery = db->Query("SELECT * from threads ORDER BY date LIMIT %lu OFFSET %lu;",
                             limit,
                             offset);

    if ( (retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ThreadsTableRow>();
    }

    std::vector<ThreadsTableRow> ret;

    do {
        ret.push_back(ThreadsTableRow{(*retQuery)[0].GetUInt32(),  // ID
                                      (*retQuery)[1].GetUInt32(),    // date
                                      (*retQuery)[2].GetUInt32(),    // msgCount
                                      (*retQuery)[3].GetUInt32(),    // msgRead
                                      (*retQuery)[4].GetUInt32(),    // contactID
                                      (*retQuery)[5].GetString(),    // snippet
                                      static_cast<SMSType >((*retQuery)[6].GetUInt32()),    // type/last-dir
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<ThreadsTableRow> ThreadsTable::GetLimitOffsetByFieldID(uint32_t offset, uint32_t limit, const char *field,
                                                                   uint32_t id) {
    auto retQuery = db->Query("SELECT * from threads WHERE %s=%lu ORDER BY date LIMIT %lu OFFSET %lu;",
                             field,
                             id,
                             limit,
                             offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ThreadsTableRow>();
    }

    std::vector<ThreadsTableRow> ret;

    do {
        ret.push_back(ThreadsTableRow{(*retQuery)[0].GetUInt32(),  // ID
                                      (*retQuery)[1].GetUInt32(),    // date
                                      (*retQuery)[2].GetUInt32(),    // msgCount
                                      (*retQuery)[3].GetUInt32(),    // msgRead
                                      (*retQuery)[4].GetUInt32(),    // contactID
                                      (*retQuery)[5].GetString(),    // snippet
                                      static_cast<SMSType >((*retQuery)[6].GetUInt32()),    // type/last-dir
        });
    } while (retQuery->NextRow());

    return ret;

}

uint32_t ThreadsTable::GetCount() {
    auto queryRet = db->Query("SELECT COUNT(*) FROM threads;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ThreadsTable::GetCountByFieldID(const char *field, uint32_t id) {
    auto queryRet = db->Query("SELECT COUNT(*) FROM threads WHERE %s=%u;",field,id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}
