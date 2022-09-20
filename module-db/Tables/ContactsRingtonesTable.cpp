// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactsRingtonesTable.hpp"
#include "Common/Types.hpp"

ContactsRingtonesTable::ContactsRingtonesTable(Database *db) : Table(db)
{}

ContactsRingtonesTable::~ContactsRingtonesTable()
{}

bool ContactsRingtonesTable::create()
{
    return true;
}

bool ContactsRingtonesTable::add(ContactsRingtonesTableRow entry)
{
    return db->execute("insert or ignore into contact_ringtones (contact_id, asset_path ) VALUES (" u32_c str_ ");",
                       entry.contactID,
                       entry.assetPath.c_str());
}

bool ContactsRingtonesTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM contact_ringtones where _id=" u32_ ";", id);
}

bool ContactsRingtonesTable::update(ContactsRingtonesTableRow entry)
{
    return db->execute("UPDATE contact_ringtones SET contact_id=" u32_c "asset_path=" str_ " WHERE _id=" u32_ ";",
                       entry.contactID,
                       entry.assetPath.c_str(),
                       entry.ID);
}

ContactsRingtonesTableRow ContactsRingtonesTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM contact_ringtones WHERE _id=" u32_ ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return ContactsRingtonesTableRow();
    }

    return ContactsRingtonesTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getUInt32(), // contactID
        (*retQuery)[2].getString()  // assetPath
    };
}

std::vector<ContactsRingtonesTableRow> ContactsRingtonesTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->query("SELECT * from contact_ringtones ORDER BY contact_id LIMIT " u32_ " OFFSET " u32_ ";", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsRingtonesTableRow>();
    }

    std::vector<ContactsRingtonesTableRow> ret;

    do {
        ret.push_back(ContactsRingtonesTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // contactID
            (*retQuery)[2].getString()  // assetPath
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
    case ContactRingtonesTableFields::AssetPath:
        fieldName = "asset_path";
        break;
    default:
        return std::vector<ContactsRingtonesTableRow>();
    }

    auto retQuery = db->query("SELECT * from contact_ringtones WHERE %q=" str_ " ORDER BY contact_id LIMIT " u32_
                              " OFFSET " u32_ ";",
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
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // contactID
            (*retQuery)[2].getString()  // assetPath
        });
    } while (retQuery->nextRow());

    return ret;
}

uint32_t ContactsRingtonesTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_ringtones;");

    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t ContactsRingtonesTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_ringtones WHERE %q=" u32_ ";", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}
