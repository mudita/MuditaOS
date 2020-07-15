
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

bool ContactsRingtonesTable::create()
{
    return db->execute(createTableQuery);
}

bool ContactsRingtonesTable::add(ContactsRingtonesTableRow entry)
{
    return db->execute("insert or ignore into contact_ringtones (contact_id, asset_path ) VALUES (%lu, '%q');",
                       entry.contactID,
                       entry.assetPath.c_str());
}

bool ContactsRingtonesTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM contact_ringtones where _id = %u;", id);
}

bool ContactsRingtonesTable::update(ContactsRingtonesTableRow entry)
{
    return db->execute("UPDATE contact_ringtones SET contact_id = %lu, asset_path = '%q' WHERE _id=%lu;",
                       entry.contactID,
                       entry.assetPath.c_str(),
                       entry.ID);
}

ContactsRingtonesTableRow ContactsRingtonesTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM contact_ringtones WHERE _id= %lu;", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return ContactsRingtonesTableRow();
    }

    return ContactsRingtonesTableRow{
        (*retQuery)[0].GetUInt32(), // ID
        (*retQuery)[1].GetUInt32(), // contactID
        (*retQuery)[2].GetString()  // assetPath
    };
}

std::vector<ContactsRingtonesTableRow> ContactsRingtonesTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->query("SELECT * from contact_ringtones ORDER BY contact_id LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsRingtonesTableRow>();
    }

    std::vector<ContactsRingtonesTableRow> ret;

    do {
        ret.push_back(ContactsRingtonesTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // contactID
            (*retQuery)[2].GetString()  // assetPath
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<ContactsRingtonesTableRow> ContactsRingtonesTable::getLimitOffsetByField(uint32_t offset,
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

    auto retQuery = db->query("SELECT * from contact_ringtones WHERE %q='%q' ORDER BY contact_id LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsRingtonesTableRow>();
    }

    std::vector<ContactsRingtonesTableRow> ret;

    do {
        ret.push_back(ContactsRingtonesTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // contactID
            (*retQuery)[2].GetString()  // assetPath
        });
    } while (retQuery->nextRow());

    return ret;
}

uint32_t ContactsRingtonesTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_ringtones;");

    if (queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ContactsRingtonesTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_ringtones WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}
