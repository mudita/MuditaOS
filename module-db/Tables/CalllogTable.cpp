// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalllogTable.hpp"
#include "Common/Types.hpp"
#include <log/log.hpp>
#include <Utils.hpp>

CalllogTable::CalllogTable(Database *db) : Table(db)
{}

CalllogTable::~CalllogTable()
{}

bool CalllogTable::create()
{
    return true;
}

bool CalllogTable::add(CalllogTableRow entry)
{
    return db->execute("INSERT or ignore INTO calls (number, e164number, presentation, date, duration, type, name, "
                       "contactId,isRead) VALUES (" str_c str_c u32_c str_c str_c u32_c str_c u32_c u32_ ");",
                       entry.number.c_str(),
                       entry.e164number.c_str(),
                       static_cast<uint32_t>(entry.presentation),
                       utils::to_string(entry.date).c_str(),
                       utils::to_string(entry.duration).c_str(),
                       static_cast<uint32_t>(entry.type),
                       entry.name.c_str(),
                       entry.contactId,
                       entry.isRead);
}

bool CalllogTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM calls where _id=" u32_ ";", id);
}

bool CalllogTable::removeByField(CalllogTableFields field, const char *str)
{
    return false; // not implemented // TODO: alek: check this
}

bool CalllogTable::update(CalllogTableRow entry)
{
    return db->execute("UPDATE calls SET number=" str_c "e164number=" str_c "presentation=" u32_c "date=" u32_c
                       "duration=" u32_c "type=" u32_c "name=" str_c "contactId=" u32_c "isRead=" u32_
                       " WHERE _id=" u32_ ";",
                       entry.number.c_str(),
                       entry.e164number.c_str(),
                       static_cast<uint32_t>(entry.presentation),
                       static_cast<uint32_t>(entry.date),
                       static_cast<uint32_t>(entry.duration),
                       static_cast<uint32_t>(entry.type),
                       entry.name.c_str(),
                       entry.contactId,
                       entry.isRead,
                       entry.ID);
}

CalllogTableRow CalllogTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM calls WHERE _id=" u32_ ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return CalllogTableRow();
    }

    return CalllogTableRow{
        {(*retQuery)[0].getUInt32()},                              // ID
        (*retQuery)[1].getString(),                                // number
        (*retQuery)[2].getString(),                                // e164number
        static_cast<PresentationType>((*retQuery)[3].getUInt32()), // presentation
        static_cast<time_t>((*retQuery)[4].getUInt64()),           // date
        static_cast<time_t>((*retQuery)[5].getUInt64()),           // duration
        static_cast<CallType>((*retQuery)[6].getUInt32()),         // type
        (*retQuery)[7].getString(),                                // name
        (*retQuery)[8].getUInt32(),                                // contactID
        static_cast<bool>((*retQuery)[9].getUInt64()),             // isRead
    };
}

std::vector<CalllogTableRow> CalllogTable::getByContactId(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM calls WHERE contactId=" u32_ ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<CalllogTableRow>();
    }

    std::vector<CalllogTableRow> ret;

    do {
        ret.push_back(CalllogTableRow{
            {(*retQuery)[0].getUInt32()},                              // ID
            (*retQuery)[1].getString(),                                // number
            (*retQuery)[2].getString(),                                // e164number
            static_cast<PresentationType>((*retQuery)[3].getUInt32()), // presentation
            static_cast<time_t>((*retQuery)[4].getUInt64()),           // date
            static_cast<time_t>((*retQuery)[5].getUInt64()),           // duration
            static_cast<CallType>((*retQuery)[6].getUInt32()),         // type
            (*retQuery)[7].getString(),                                // name
            (*retQuery)[8].getUInt32(),                                // contactID
            static_cast<bool>((*retQuery)[9].getUInt64()),             // isRead
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<CalllogTableRow> CalllogTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from calls ORDER BY date DESC LIMIT " u32_ " OFFSET " u32_ ";", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<CalllogTableRow>();
    }

    std::vector<CalllogTableRow> ret;

    do {
        ret.push_back(CalllogTableRow{
            {(*retQuery)[0].getUInt32()},                              // ID
            (*retQuery)[1].getString(),                                // number
            (*retQuery)[2].getString(),                                // e164number
            static_cast<PresentationType>((*retQuery)[3].getUInt32()), // presentation
            static_cast<time_t>((*retQuery)[4].getUInt64()),           // date
            static_cast<time_t>((*retQuery)[5].getUInt64()),           // duration
            static_cast<CallType>((*retQuery)[6].getUInt32()),         // type
            (*retQuery)[7].getString(),                                // name
            (*retQuery)[8].getUInt32(),                                // contactID
            static_cast<bool>((*retQuery)[9].getUInt64()),             // isRead
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<CalllogTableRow> CalllogTable::getLimitOffsetByField(uint32_t offset,
                                                                 uint32_t limit,
                                                                 CalllogTableFields field,
                                                                 const char *str)
{

    std::string fieldName; // TODO: alek: add more fields?
    switch (field) {
    case CalllogTableFields::DATE:
        fieldName = "date";
        break;
    case CalllogTableFields::TYPE:
        fieldName = "type";
        break;
    default:
        return std::vector<CalllogTableRow>();
    }

    auto retQuery = db->query("SELECT * from calls WHERE %q=" u32_ " ORDER BY date LIMIT " u32_ " OFFSET " u32_ ";",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<CalllogTableRow>();
    }

    std::vector<CalllogTableRow> ret;

    do {
        ret.push_back(CalllogTableRow{
            {(*retQuery)[0].getUInt32()},                              // ID
            (*retQuery)[1].getString(),                                // number
            (*retQuery)[2].getString(),                                // e164number
            static_cast<PresentationType>((*retQuery)[3].getUInt32()), // presentation
            static_cast<time_t>((*retQuery)[4].getUInt64()),           // date
            static_cast<time_t>((*retQuery)[5].getUInt64()),           // duration
            static_cast<CallType>((*retQuery)[6].getUInt32()),         // type
            (*retQuery)[7].getString(),                                // name
            (*retQuery)[8].getUInt32(),                                // contactID
            static_cast<bool>((*retQuery)[9].getUInt64()),             // isRead
        });
    } while (retQuery->nextRow());

    return ret;
}

uint32_t CalllogTable::count(EntryState state)
{
    std::string query = "SELECT COUNT(*) FROM calls ";
    switch (state) {
    case EntryState::ALL:
        break;
    case EntryState::UNREAD:
        query += "WHERE calls.isRead = 0";
        break;
    case EntryState::READ:
        query += "WHERE calls.isRead = 1";
        break;
    }
    query += ";";
    auto queryRet = db->query(query.c_str());

    if (queryRet == nullptr || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t CalllogTable::count()
{
    return count(EntryState::ALL);
}

uint32_t CalllogTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM calls WHERE %q=" u32_ ";", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

bool CalllogTable::SetAllRead()
{
    return db->execute("UPDATE calls SET isRead = 1;");
}

bool CalllogTable::DeleteAll()
{
    return db->execute("DELETE FROM calls;");
}
