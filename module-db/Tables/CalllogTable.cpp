/*
 * @file CalllogTable.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB Table
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CalllogTable.hpp"
#include "log/log.hpp"
#include <Utils.hpp>

CalllogTable::CalllogTable(Database *db) : Table(db) {
}

CalllogTable::~CalllogTable() {

}

bool CalllogTable::Create() {
    return db->Execute(createTableQuery);
}

bool CalllogTable::Add(CalllogTableRow entry)
{

    return db->Execute("INSERT or ignore INTO calls (number, presentation, date, duration, type, name, contactId) VALUES ('%s', %lu, %s, %s, %lu, '%s', '%s');",
                       entry.number.c_str(), static_cast<uint32_t>(entry.presentation), utils::to_string(entry.date).c_str(),
                       utils::to_string(entry.duration).c_str(), static_cast<uint32_t>(entry.type), entry.name.c_str(), entry.contactId.c_str());
}

bool CalllogTable::RemoveByID(uint32_t id) {
    return db->Execute("DELETE FROM calls where _id = %lu;", id);
}

bool CalllogTable::RemoveByField(CalllogTableFields field, const char *str) {
    return false; // not implemented // TODO: alek: check this
}

bool CalllogTable::Update(CalllogTableRow entry)
{
    return db->Execute(
        "UPDATE calls SET number = '%s', presentation = %lu, date = %lu, duration = %lu, type = %lu, name = '%s', contactId = '%s' WHERE _id = %lu;",
        entry.number.c_str(), static_cast<uint32_t>(entry.presentation), utils::to_string(entry.date).c_str(), utils::to_string(entry.duration).c_str(),
        static_cast<uint32_t>(entry.type), entry.name.c_str(), entry.contactId.c_str());
}

CalllogTableRow CalllogTable::GetByID(uint32_t id) {
    auto retQuery = db->Query("SELECT * FROM calls WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return CalllogTableRow();
    }

    return CalllogTableRow{
        (*retQuery)[0].GetUInt32(),                                // ID
        (*retQuery)[1].GetString(),                                // number
        static_cast<PresentationType>((*retQuery)[2].GetUInt32()), // presentation
        static_cast<time_t>((*retQuery)[3].GetUInt64()),           // date
        static_cast<time_t>((*retQuery)[4].GetUInt64()),           // duration
        static_cast<CallType>((*retQuery)[5].GetUInt32()),         // type
        (*retQuery)[6].GetString(),                                // name
        (*retQuery)[7].GetString(),                                // contactID
    };
}

std::vector<CalllogTableRow> CalllogTable::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto retQuery = db->Query("SELECT * from calls ORDER BY date DESC LIMIT %lu OFFSET %lu;",
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<CalllogTableRow>();
    }

    std::vector<CalllogTableRow> ret;

    do {
        ret.push_back(CalllogTableRow{
            (*retQuery)[0].GetUInt32(),                                // ID
            (*retQuery)[1].GetString(),                                // number
            static_cast<PresentationType>((*retQuery)[2].GetUInt32()), // presentation
            static_cast<time_t>((*retQuery)[3].GetUInt64()),           // date
            static_cast<time_t>((*retQuery)[4].GetUInt64()),           // duration
            static_cast<CallType>((*retQuery)[5].GetUInt32()),         // type
            (*retQuery)[6].GetString(),                                // name
            (*retQuery)[7].GetString(),                                // contactID
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<CalllogTableRow>
CalllogTable::GetLimitOffsetByField(uint32_t offset, uint32_t limit, CalllogTableFields field, const char *str) {

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

    auto retQuery = db->Query("SELECT * from calls WHERE %s='%s' ORDER BY date LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<CalllogTableRow>();
    }

    std::vector<CalllogTableRow> ret;

    do {
        ret.push_back(CalllogTableRow{
            (*retQuery)[0].GetUInt32(),                                // ID
            (*retQuery)[1].GetString(),                                // number
            static_cast<PresentationType>((*retQuery)[2].GetUInt32()), // presentation
            static_cast<time_t>((*retQuery)[3].GetUInt64()),           // date
            static_cast<time_t>((*retQuery)[4].GetUInt64()),           // duration
            static_cast<CallType>((*retQuery)[5].GetUInt32()),         // type
            (*retQuery)[6].GetString(),                                // name
            (*retQuery)[7].GetString(),                                // contactID
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t CalllogTable::GetCount() {
    auto queryRet = db->Query("SELECT COUNT(*) FROM calls;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t CalllogTable::GetCountByFieldID(const char *field, uint32_t id) {
    auto queryRet = db->Query("SELECT COUNT(*) FROM calls WHERE %s=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

