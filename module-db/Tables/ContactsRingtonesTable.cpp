
/*
 * @file ContactsRingtonesTable.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "ContactsRingtonesTable.hpp"

ContactsRingtonesTable::ContactsRingtonesTable(Database *db) : Table(db)
{}

ContactsRingtonesTable::~ContactsRingtonesTable()
{}

bool ContactsRingtonesTable::Create()
{
    return db->Execute(createTableQuery);
}

bool ContactsRingtonesTable::Add(ContactsRingtonesTableRow entry)
{
    return db->Execute("insert or ignore into contact_ringtones (contact_id, asset_path ) VALUES (%lu, '%s');",
                       entry.contactID,
                       entry.assetPath.c_str());
}

bool ContactsRingtonesTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM contact_ringtones where _id = %u;", id);
}

bool ContactsRingtonesTable::Update(ContactsRingtonesTableRow entry)
{
    return db->Execute("UPDATE contact_ringtones SET contact_id = %lu, asset_path = '%s' WHERE _id=%lu;",
                       entry.contactID,
                       entry.assetPath.c_str(),
                       entry.ID);
}

ContactsRingtonesTableRow ContactsRingtonesTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM contact_ringtones WHERE _id= %lu;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return ContactsRingtonesTableRow();
    }

    return ContactsRingtonesTableRow{
        (*retQuery)[0].GetUInt32(), // ID
        (*retQuery)[1].GetUInt32(), // contactID
        (*retQuery)[2].GetString()  // assetPath
    };
}

std::vector<ContactsRingtonesTableRow> ContactsRingtonesTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->Query("SELECT * from contact_ringtones ORDER BY contact_id LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsRingtonesTableRow>();
    }

    std::vector<ContactsRingtonesTableRow> ret;

    do {
        ret.push_back(ContactsRingtonesTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // contactID
            (*retQuery)[2].GetString()  // assetPath
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<ContactsRingtonesTableRow> ContactsRingtonesTable::GetLimitOffsetByField(uint32_t offset,
                                                                                     uint32_t limit,
                                                                                     ContactRingtonesTableFields field,
                                                                                     const char *str)
{
    std::string fieldName;
    switch (field) {
    case ContactRingtonesTableFields ::AssetPath:
        fieldName = "asset_path";
        break;
    default:
        return std::vector<ContactsRingtonesTableRow>();
    }

    auto retQuery = db->Query("SELECT * from contact_ringtones WHERE %s='%s' ORDER BY contact_id LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsRingtonesTableRow>();
    }

    std::vector<ContactsRingtonesTableRow> ret;

    do {
        ret.push_back(ContactsRingtonesTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // contactID
            (*retQuery)[2].GetString()  // assetPath
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t ContactsRingtonesTable::GetCount()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_ringtones;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ContactsRingtonesTable::GetCountByFieldID(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_ringtones WHERE %s=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}
