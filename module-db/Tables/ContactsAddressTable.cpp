// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactsAddressTable.hpp"
#include "Common/Types.hpp"

ContactsAddressTable::ContactsAddressTable(Database *db) : Table(db)
{}

ContactsAddressTable::~ContactsAddressTable()
{}

bool ContactsAddressTable::create()
{
    return true;
}

bool ContactsAddressTable::add(ContactsAddressTableRow entry)
{
    return db->execute("insert or ignore into contact_address (contact_id, address, note, mail) "
                       "VALUES (" u32_c str_c str_c str_ ");",
                       entry.contactID,
                       entry.address.c_str(),
                       entry.note.c_str(),
                       entry.mail.c_str());
}

bool ContactsAddressTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM contact_address where _id=" u32_ ";", id);
}

bool ContactsAddressTable::update(ContactsAddressTableRow entry)
{
    return db->execute("UPDATE contact_address SET contact_id=" u32_c "address=" str_c "note=" str_c "mail=" str_
                       "WHERE _id=" u32_ ";",
                       entry.contactID,
                       entry.address.c_str(),
                       entry.note.c_str(),
                       entry.mail.c_str(),
                       entry.ID);
}

ContactsAddressTableRow ContactsAddressTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM contact_address WHERE _id=" u32_ ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return ContactsAddressTableRow();
    }

    return ContactsAddressTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getUInt32(), // contactID
        (*retQuery)[2].getString(), // address
        (*retQuery)[3].getString(), // note
        (*retQuery)[4].getString(), // mail
    };
}

std::vector<ContactsAddressTableRow> ContactsAddressTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->query("SELECT * from contact_address ORDER BY contact_id LIMIT " u32_ " OFFSET " u32_ ";", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsAddressTableRow>();
    }

    std::vector<ContactsAddressTableRow> ret;

    do {
        ret.push_back(ContactsAddressTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // contactID
            (*retQuery)[2].getString(), // address
            (*retQuery)[3].getString(), // note
            (*retQuery)[4].getString(), // mail
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<ContactsAddressTableRow> ContactsAddressTable::getLimitOffsetByField(uint32_t offset,
                                                                                 uint32_t limit,
                                                                                 ContactAddressTableFields field,
                                                                                 const char *str)
{

    std::string fieldName;
    switch (field) {
    case ContactAddressTableFields::Mail:
        fieldName = "mail";
        break;
    default:
        return std::vector<ContactsAddressTableRow>();
    }

    auto retQuery =
        db->query("SELECT * from contact_address WHERE %q=" str_ " ORDER BY contact_id LIMIT " u32_ " OFFSET " u32_ ";",
                  fieldName.c_str(),
                  str,
                  limit,
                  offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsAddressTableRow>();
    }

    std::vector<ContactsAddressTableRow> ret;

    do {
        ret.push_back(ContactsAddressTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // contactID
            (*retQuery)[2].getString(), // address
            (*retQuery)[3].getString(), // note
            (*retQuery)[4].getString(), // mail
        });
    } while (retQuery->nextRow());

    return ret;
}

uint32_t ContactsAddressTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_address;");

    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t ContactsAddressTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_address WHERE %q=" u32_ ";", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}
