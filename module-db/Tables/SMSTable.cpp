#include "SMSTable.hpp"
#include <log/log.hpp>

SMSTable::SMSTable(Database *db) : Table(db)
{}

bool SMSTable::create()
{
    return db->execute(createTableQuery);
}

bool SMSTable::add(SMSTableRow entry)
{
    return db->execute("INSERT or ignore INTO sms ( thread_id,contact_id, date, date_send, error_code, body, "
                       "type ) VALUES (%lu,%lu,%lu,%lu,0,'%q',%d);",
                       entry.threadID,
                       entry.contactID,
                       entry.date,
                       entry.dateSent,
                       entry.body.c_str(),
                       entry.type);
}

bool SMSTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM sms where _id = %u;", id);
}

bool SMSTable::removeByField(SMSTableFields field, const char *str)
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

    return db->execute("DELETE FROM sms where %q = '%q';", fieldName.c_str(), str);
}

bool SMSTable::update(SMSTableRow entry)
{
    return db->execute("UPDATE sms SET thread_id = %lu, contact_id = %lu ,date = %lu, date_send = %lu, error_code = 0, "
                       "body = '%q', type =%d WHERE _id=%lu;",
                       entry.threadID,
                       entry.contactID,
                       entry.date,
                       entry.dateSent,
                       entry.body.c_str(),
                       entry.type,
                       entry.ID);
}

SMSTableRow SMSTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM sms WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return SMSTableRow();
    }

    return SMSTableRow{
        (*retQuery)[0].getUInt32(),                       // ID
        (*retQuery)[1].getUInt32(),                       // threadID
        (*retQuery)[2].getUInt32(),                       // contactID
        (*retQuery)[3].getUInt32(),                       // date
        (*retQuery)[4].getUInt32(),                       // dateSent
        (*retQuery)[5].getUInt32(),                       // errorCode
        (*retQuery)[6].getString(),                       // body
        static_cast<SMSType>((*retQuery)[7].getUInt32()), // type
    };
}

std::vector<SMSTableRow> SMSTable::getByContactId(uint32_t contactId)
{
    auto retQuery = db->query("SELECT * FROM sms WHERE contact_id= %u;", contactId);

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
            (*retQuery)[4].getUInt32(),                       // dateSent
            (*retQuery)[5].getUInt32(),                       // errorCode
            (*retQuery)[6].getString(),                       // body
            static_cast<SMSType>((*retQuery)[7].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}
std::vector<SMSTableRow> SMSTable::getByThreadId(uint32_t threadId)
{
    auto retQuery = db->query("SELECT * FROM sms WHERE thread_id= %u;", threadId);

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
            (*retQuery)[4].getUInt32(),                       // dateSent
            (*retQuery)[5].getUInt32(),                       // errorCode
            (*retQuery)[6].getString(),                       // body
            static_cast<SMSType>((*retQuery)[7].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<SMSTableRow> SMSTable::getByText(std::string text)
{

    auto retQuery = db->query("SELECT *, INSTR(body,'%s') pos FROM sms WHERE pos > 0;", text.c_str());

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
            (*retQuery)[4].getUInt32(),                       // dateSent
            (*retQuery)[5].getUInt32(),                       // errorCode
            (*retQuery)[6].getString(),                       // body
            static_cast<SMSType>((*retQuery)[7].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<SMSTableRow> SMSTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from sms ORDER BY date DESC LIMIT %lu OFFSET %lu;", limit, offset);

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
            (*retQuery)[4].getUInt32(),                       // dateSent
            (*retQuery)[5].getUInt32(),                       // errorCode
            (*retQuery)[6].getString(),                       // body
            static_cast<SMSType>((*retQuery)[7].getUInt32()), // type
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
    case SMSTableFields ::ContactID:
        fieldName = "contact_id";
        break;
    case SMSTableFields ::ThreadID:
        fieldName = "thread_id";
        break;
    default:
        return std::vector<SMSTableRow>();
    }

    auto retQuery = db->query("SELECT * from sms WHERE %q='%q' ORDER BY date DESC LIMIT %lu OFFSET %lu;",
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
            (*retQuery)[4].getUInt32(),                       // dateSent
            (*retQuery)[5].getUInt32(),                       // errorCode
            (*retQuery)[6].getString(),                       // body
            static_cast<SMSType>((*retQuery)[7].getUInt32()), // type
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
    auto queryRet = db->query("SELECT COUNT(*) FROM sms WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

std::pair<uint32_t, std::vector<SMSTableRow>> SMSTable::getManyByType(SMSType type, uint32_t offset, uint32_t limit)
{
    auto ret   = std::pair<uint32_t, std::vector<SMSTableRow>>{0, {}};
    auto count = db->query("SELECT COUNT (*) from sms WHERE type='%lu';", type);
    ret.first  = count == nullptr ? 0 : (*count)[0].getUInt32();
    if (ret.first != 0) {
        limit         = limit == 0 ? ret.first : limit; // no limit intended
        auto retQuery = db->query(
            "SELECT * from sms WHERE type='%lu' ORDER BY date ASC LIMIT %lu OFFSET %lu;", type, limit, offset);

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
                    (*retQuery)[4].getUInt32(),                       // dateSent
                    (*retQuery)[5].getUInt32(),                       // errorCode
                    (*retQuery)[6].getString(),                       // body
                    static_cast<SMSType>((*retQuery)[7].getUInt32()), // type
                });
            } while (retQuery->nextRow());
        }
    }
    else {
        ret.second = {};
    }
    return ret;
}
