
/*
 * @file ContactsNumberTable.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "ContactsNumberTable.hpp"

ContactsNumberTable::ContactsNumberTable(Database *db) : Table(db)
{
}

ContactsNumberTable::~ContactsNumberTable()
{
}

bool ContactsNumberTable::Create()
{
    return db->Execute(createTableQuery);
}

bool ContactsNumberTable::Add(ContactsNumberTableRow entry)
{
    return db->Execute("insert or ignore into contact_number (contact_id, number_user, number_e164, type) VALUES (%lu, '%s', '%s', %lu);", entry.contactID,
                       entry.numberUser.c_str(), entry.numbere164.c_str(), entry.type);
}

bool ContactsNumberTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM contact_number where _id = %u;", id);
}

bool ContactsNumberTable::Update(ContactsNumberTableRow entry)
{
    return db->Execute("UPDATE contact_number SET contact_id = %lu, number_user = '%s' ,number_e164 = '%s', type = %lu WHERE _id=%lu;", entry.contactID,
                       entry.numberUser.c_str(), entry.numbere164.c_str(), entry.type, entry.ID);
}

ContactsNumberTableRow ContactsNumberTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM contact_number WHERE _id= %lu;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0))
    {
        return ContactsNumberTableRow();
    }

    return ContactsNumberTableRow{
        (*retQuery)[0].GetUInt32(),                                 // ID
        (*retQuery)[1].GetUInt32(),                                 // contactID
        (*retQuery)[2].GetString(),                                 // numberUser
        (*retQuery)[3].GetString(),                                 // numbere164
        static_cast<ContactNumberType>((*retQuery)[4].GetUInt32()), // type
    };
}

std::vector<ContactsNumberTableRow> ContactsNumberTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->Query("SELECT * from contact_number ORDER BY number_user LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0))
    {
        return std::vector<ContactsNumberTableRow>();
    }

    std::vector<ContactsNumberTableRow> ret;

    do
    {
        ret.push_back(ContactsNumberTableRow{
            (*retQuery)[0].GetUInt32(),                                 // ID
            (*retQuery)[1].GetUInt32(),                                 // contactID
            (*retQuery)[2].GetString(),                                 // numberUser
            (*retQuery)[3].GetString(),                                 // numbere164
            static_cast<ContactNumberType>((*retQuery)[4].GetUInt32()), // type
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<ContactsNumberTableRow> ContactsNumberTable::GetLimitOffsetByField(uint32_t offset, uint32_t limit, ContactNumberTableFields field, const char *str)
{
    std::string fieldName;
    switch (field)
    {
    case ContactNumberTableFields ::NumberUser:
        fieldName = "number_user";
        break;
    case ContactNumberTableFields ::NumberE164:
        fieldName = "number_e164";
        break;
    default:
        return std::vector<ContactsNumberTableRow>();
    }

    auto retQuery = db->Query("SELECT * from contact_number WHERE %s='%s' ORDER BY number_user LIMIT %lu OFFSET %lu;", fieldName.c_str(), str, limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0))
    {
        return std::vector<ContactsNumberTableRow>();
    }

    std::vector<ContactsNumberTableRow> ret;

    do
    {
        ret.push_back(ContactsNumberTableRow{
            (*retQuery)[0].GetUInt32(),                                 // ID
            (*retQuery)[1].GetUInt32(),                                 // contactID
            (*retQuery)[2].GetString(),                                 // numberUser
            (*retQuery)[3].GetString(),                                 // numbere164
            static_cast<ContactNumberType>((*retQuery)[4].GetUInt32()), // type
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t ContactsNumberTable::GetCount()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_number;");

    if (queryRet->GetRowCount() == 0)
    {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ContactsNumberTable::GetCountByFieldID(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_number WHERE %s=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0))
    {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}