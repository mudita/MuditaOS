
/*
 * @file SMSTable.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "SMSTable.hpp"


SMSTable::SMSTable(Database* db): db(db){
}

SMSTable::~SMSTable() {

}

bool SMSTable::Create() {
    return db->Execute(createTableQuery);
}

bool SMSTable::Add(SMSTableRow entry) {
    return db->Execute(
            "INSERT INTO sms ( thread_id,contact_id, date, date_send, error_code, body, read, type ) VALUES (%lu,%lu,%lu,%lu,0,'%s',%d,%d);",
            entry.threadID,
            entry.contactID,
            entry.date,
            entry.dateSent,
            entry.body.c_str(),
            entry.isRead,
            entry.type
    );
}

bool SMSTable::RemoveByID(uint32_t id) {
    return db->Execute("DELETE FROM sms where _id = %u;", id);
}


bool SMSTable::Update(SMSTableRow entry) {
    return db->Execute(
            "UPDATE sms SET thread_id = %lu, contact_id = %lu ,date = %lu, date_send = %lu, error_code = 0, body = '%s', read = %d, type =%d WHERE _id=%lu;",
            entry.threadID,
            entry.contactID,
            entry.date,
            entry.dateSent,
            entry.body.c_str(),
            entry.isRead,
            entry.type,
            entry.ID
    );
}

SMSTableRow SMSTable::GetByID(uint32_t id) {
    auto retQuery = db->Query("SELECT * FROM sms WHERE _id= %u;", id);

    if (retQuery->GetRowCount() == 0) {
        return SMSTableRow();
    }

    return SMSTableRow{(*retQuery)[0].GetUInt32(),  // ID
                       (*retQuery)[1].GetUInt32(),    // threadID
                       (*retQuery)[2].GetUInt32(),    // contactID
                       (*retQuery)[3].GetUInt32(),    // date
                       (*retQuery)[4].GetUInt32(),    // dateSent
                       (*retQuery)[5].GetUInt32(),    // errorCode
                       (*retQuery)[6].GetString(),    // body
                       (*retQuery)[7].GetBool(),      // isRead
                       static_cast<SMSType >((*retQuery)[8].GetUInt32()),    // type
    };
}

std::vector<SMSTableRow> SMSTable::GetLimitOffset(uint32_t offset, uint32_t limit) {

    auto retQuery = db->Query("SELECT * from sms ORDER BY date LIMIT %lu OFFSET %lu;",
                             limit,
                             offset);

    if (retQuery->GetRowCount() == 0) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{(*retQuery)[0].GetUInt32(),  // ID
                                  (*retQuery)[1].GetUInt32(),    // threadID
                                  (*retQuery)[2].GetUInt32(),    // contactID
                                  (*retQuery)[3].GetUInt32(),    // date
                                  (*retQuery)[4].GetUInt32(),    // dateSent
                                  (*retQuery)[5].GetUInt32(),    // errorCode
                                  (*retQuery)[6].GetString(),    // body
                                  (*retQuery)[7].GetBool(),      // isRead
                                  static_cast<SMSType >((*retQuery)[8].GetUInt32()),    // type
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<SMSTableRow> SMSTable::GetLimitOffsetByFieldID(uint32_t offset, uint32_t limit, const char *field, uint32_t id) {
    auto retQuery = db->Query("SELECT * from sms WHERE %s=%lu ORDER BY date LIMIT %lu OFFSET %lu;",
                             field,
                             id,
                             limit,
                             offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{(*retQuery)[0].GetUInt32(),  // ID
                                  (*retQuery)[1].GetUInt32(),    // threadID
                                  (*retQuery)[2].GetUInt32(),    // contactID
                                  (*retQuery)[3].GetUInt32(),    // date
                                  (*retQuery)[4].GetUInt32(),    // dateSent
                                  (*retQuery)[5].GetUInt32(),    // errorCode
                                  (*retQuery)[6].GetString(),    // body
                                  (*retQuery)[7].GetBool(),      // isRead
                                  static_cast<SMSType >((*retQuery)[8].GetUInt32()),    // type
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t SMSTable::GetCount() {
    auto queryRet = db->Query("SELECT COUNT(*) FROM SMS;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t SMSTable::GetCountByFieldID(const char *field, uint32_t id) {
    auto queryRet = db->Query("SELECT COUNT(*) FROM sms WHERE %s=%lu;",field,id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}
