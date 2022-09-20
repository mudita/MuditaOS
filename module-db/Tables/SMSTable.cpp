// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSTable.hpp"
#include "Common/Types.hpp"
#include <log/log.hpp>

SMSTable::SMSTable(Database *db) : Table(db)
{}

bool SMSTable::create()
{
    return true;
}

bool SMSTable::add(SMSTableRow entry)
{
    return db->execute("INSERT or ignore INTO sms ( thread_id,contact_id, date, error_code, body, "
                       "type ) VALUES (" u32_c u32_c u32_c "0," str_c u32_ ");",
                       entry.threadID,
                       entry.contactID,
                       entry.date,
                       entry.body.c_str(),
                       entry.type);
}

bool SMSTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM sms where _id=" u32_ ";", id);
}

bool SMSTable::removeByField(SMSTableFields field, const char *str)
{
    std::string fieldName;

    switch (field) {
    case SMSTableFields::ThreadID:
        fieldName = "thread_id";
        break;

    case SMSTableFields::ContactID:
        fieldName = "contact_id";
        break;

    case SMSTableFields::Date:
        fieldName = "date";
        break;
    default:
        return false;
    }

    return db->execute("DELETE FROM sms where %q=" u32_ ";", fieldName.c_str(), str);
}

bool SMSTable::update(SMSTableRow entry)
{
    return db->execute("UPDATE sms SET thread_id=" u32_c "contact_id=" u32_c "date=" u32_c "error_code=0,"
                       "body=" str_c "type=" u32_ " WHERE _id=" u32_ ";",
                       entry.threadID,
                       entry.contactID,
                       entry.date,
                       entry.body.c_str(),
                       entry.type,
                       entry.ID);
}

SMSTableRow SMSTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM sms WHERE _id=" u32_ ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return SMSTableRow();
    }

    return SMSTableRow{
        (*retQuery)[0].getUInt32(),                       // ID
        (*retQuery)[1].getUInt32(),                       // threadID
        (*retQuery)[2].getUInt32(),                       // contactID
        (*retQuery)[3].getUInt32(),                       // date
        (*retQuery)[4].getUInt32(),                       // errorCode
        (*retQuery)[5].getString(),                       // body
        static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
    };
}

std::vector<SMSTableRow> SMSTable::getByContactId(uint32_t contactId)
{
    auto retQuery = db->query("SELECT * FROM sms WHERE contact_id=" u32_ ";", contactId);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{
            (*retQuery)[0].getUInt32(),                       // ID
            (*retQuery)[1].getUInt32(),                       // threadID
            (*retQuery)[2].getUInt32(),                       // contactID
            (*retQuery)[3].getUInt32(),                       // date
            (*retQuery)[4].getUInt32(),                       // errorCode
            (*retQuery)[5].getString(),                       // body
            static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}
std::vector<SMSTableRow> SMSTable::getByThreadId(uint32_t threadId, uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * FROM sms WHERE thread_id=" u32_ ";", threadId);

    if (limit != 0) {
        retQuery = db->query(
            "SELECT * FROM sms WHERE thread_id=" u32_ " LIMIT " u32_ " OFFSET " u32_ ";", threadId, limit, offset);
    }

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{
            (*retQuery)[0].getUInt32(),                       // ID
            (*retQuery)[1].getUInt32(),                       // threadID
            (*retQuery)[2].getUInt32(),                       // contactID
            (*retQuery)[3].getUInt32(),                       // date
            (*retQuery)[4].getUInt32(),                       // errorCode
            (*retQuery)[5].getString(),                       // body
            static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<SMSTableRow> SMSTable::getByThreadIdWithoutDraftWithEmptyInput(uint32_t threadId,
                                                                           uint32_t offset,
                                                                           uint32_t limit)
{
    auto retQuery =
        db->query("SELECT * FROM sms WHERE thread_id=" u32_ " AND type!=" u32_ " UNION ALL SELECT 0 as _id, 0 as "
                  "thread_id, 0 as contact_id, 0 as "
                  "date, 0 as error_code, 0 as body, " u32_ " as type LIMIT " u32_ " OFFSET " u32_,
                  threadId,
                  SMSType::DRAFT,
                  SMSType::INPUT,
                  limit,
                  offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{
            (*retQuery)[0].getUInt32(),                       // ID
            (*retQuery)[1].getUInt32(),                       // threadID
            (*retQuery)[2].getUInt32(),                       // contactID
            (*retQuery)[3].getUInt32(),                       // date
            (*retQuery)[4].getUInt32(),                       // errorCode
            (*retQuery)[5].getString(),                       // body
            static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}

uint32_t SMSTable::countWithoutDraftsByThreadId(uint32_t threadId)
{
    auto queryRet =
        db->query("SELECT COUNT(*) FROM sms WHERE thread_id=" u32_ " AND type!=" u32_ ";", threadId, SMSType::DRAFT);

    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

SMSTableRow SMSTable::getDraftByThreadId(uint32_t threadId)
{
    auto retQuery =
        db->query("SELECT * FROM sms WHERE thread_id=" u32_ " AND type=" u32_ " ORDER BY date DESC LIMIT 1;",
                  threadId,
                  SMSType::DRAFT);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return SMSTableRow();
    }

    return SMSTableRow{
        (*retQuery)[0].getUInt32(),                       // ID
        (*retQuery)[1].getUInt32(),                       // threadID
        (*retQuery)[2].getUInt32(),                       // contactID
        (*retQuery)[3].getUInt32(),                       // date
        (*retQuery)[4].getUInt32(),                       // errorCode
        (*retQuery)[5].getString(),                       // body
        static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
    };
}

std::vector<SMSTableRow> SMSTable::getByText(std::string text)
{

    auto retQuery = db->query("SELECT *, INSTR(body," str_ ") pos FROM sms WHERE pos > 0;", text.c_str());

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{
            (*retQuery)[0].getUInt32(),                       // ID
            (*retQuery)[1].getUInt32(),                       // threadID
            (*retQuery)[2].getUInt32(),                       // contactID
            (*retQuery)[3].getUInt32(),                       // date
            (*retQuery)[4].getUInt32(),                       // errorCode
            (*retQuery)[5].getString(),                       // body
            static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<SMSTableRow> SMSTable::getByText(std::string text, uint32_t threadId)
{
    auto retQuery = db->query(
        "SELECT *, INSTR(body," str_ ") pos FROM sms WHERE pos > 0 AND thread_id=" u32_ ";", text.c_str(), threadId);
    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return {};
    }

    std::vector<SMSTableRow> ret;
    do {
        ret.push_back(SMSTableRow{
            (*retQuery)[0].getUInt32(),                       // ID
            (*retQuery)[1].getUInt32(),                       // threadID
            (*retQuery)[2].getUInt32(),                       // contactID
            (*retQuery)[3].getUInt32(),                       // date
            (*retQuery)[4].getUInt32(),                       // errorCode
            (*retQuery)[5].getString(),                       // body
            static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
        });
    } while (retQuery->nextRow());
    return ret;
}

std::vector<SMSTableRow> SMSTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from sms ORDER BY date DESC LIMIT " u32_ " OFFSET " u32_ ";", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{
            (*retQuery)[0].getUInt32(),                       // ID
            (*retQuery)[1].getUInt32(),                       // threadID
            (*retQuery)[2].getUInt32(),                       // contactID
            (*retQuery)[3].getUInt32(),                       // date
            (*retQuery)[4].getUInt32(),                       // errorCode
            (*retQuery)[5].getString(),                       // body
            static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<SMSTableRow> SMSTable::getLimitOffsetByField(uint32_t offset,
                                                         uint32_t limit,
                                                         SMSTableFields field,
                                                         const char *str)
{

    std::string fieldName;
    switch (field) {
    case SMSTableFields::Date:
        fieldName = "date";
        break;
    case SMSTableFields::ContactID:
        fieldName = "contact_id";
        break;
    case SMSTableFields::ThreadID:
        fieldName = "thread_id";
        break;
    default:
        return std::vector<SMSTableRow>();
    }

    auto retQuery = db->query("SELECT * from sms WHERE %q=" str_ " ORDER BY date DESC LIMIT " u32_ " OFFSET " u32_ ";",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<SMSTableRow>();
    }

    std::vector<SMSTableRow> ret;

    do {
        ret.push_back(SMSTableRow{
            {(*retQuery)[0].getUInt32()},                     // ID
            (*retQuery)[1].getUInt32(),                       // threadID
            (*retQuery)[2].getUInt32(),                       // contactID
            (*retQuery)[3].getUInt32(),                       // date
            (*retQuery)[4].getUInt32(),                       // errorCode
            (*retQuery)[5].getString(),                       // body
            static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}
uint32_t SMSTable::count()
{
    std::string query = "SELECT COUNT(*) FROM sms;";

    auto queryRet = db->query(query.c_str());
    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t SMSTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM sms WHERE %q=" u32_ ";", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

std::pair<uint32_t, std::vector<SMSTableRow>> SMSTable::getManyByType(SMSType type, uint32_t offset, uint32_t limit)
{
    auto ret   = std::pair<uint32_t, std::vector<SMSTableRow>>{0, {}};
    auto count = db->query("SELECT COUNT (*) from sms WHERE type=" u32_ ";", type);
    ret.first  = count == nullptr ? 0 : (*count)[0].getUInt32();
    if (ret.first != 0) {
        limit = limit == 0 ? ret.first : limit; // no limit intended
        auto retQuery =
            db->query("SELECT * from sms WHERE type=" u32_ " ORDER BY date ASC LIMIT " u32_ " OFFSET " u32_ ";",
                      type,
                      limit,
                      offset);

        if (retQuery == nullptr || retQuery->getRowCount() == 0) {
            ret.second = {};
        }
        else {
            do {
                ret.second.push_back(SMSTableRow{
                    {(*retQuery)[0].getUInt32()},                     // ID
                    (*retQuery)[1].getUInt32(),                       // threadID
                    (*retQuery)[2].getUInt32(),                       // contactID
                    (*retQuery)[3].getUInt32(),                       // date
                    (*retQuery)[4].getUInt32(),                       // errorCode
                    (*retQuery)[5].getString(),                       // body
                    static_cast<SMSType>((*retQuery)[6].getUInt32()), // type
                });
            } while (retQuery->nextRow());
        }
    }
    else {
        ret.second = {};
    }
    return ret;
}
