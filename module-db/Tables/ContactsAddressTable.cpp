
/*
 * @file ContactsAddressTable.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ContactsAddressTable.hpp"


ContactsAddressTable::ContactsAddressTable(Database *db): Table(db) {

}

ContactsAddressTable::~ContactsAddressTable() {

}

bool ContactsAddressTable::Create() {
    return db->Execute(createTableQuery);
}

bool ContactsAddressTable::Add(ContactsAddressTableRow entry) {
    return db->Execute(
            "insert into contact_address (contact_id, country, city, street, number, type, note, mail) VALUES (%lu, '%s', '%s', '%s', '%s', %lu, '%s', '%s');",
            entry.contactID,
            entry.country.c_str(),
            entry.city.c_str(),
            entry.street.c_str(),
            entry.number.c_str(),
            entry.type,
            entry.note.c_str(),
            entry.mail.c_str()
    );
}

bool ContactsAddressTable::RemoveByID(uint32_t id) {
    return db->Execute("DELETE FROM contact_address where _id = %u;", id);
}

bool ContactsAddressTable::Update(ContactsAddressTableRow entry) {
    return db->Execute(
            "UPDATE contact_address SET contact_id = %lu, country = '%s' ,city = '%s',street = '%s',number = '%s', type =%lu, note = '%s',mail = '%s' WHERE _id=%lu;",
            entry.contactID,
            entry.country.c_str(),
            entry.city.c_str(),
            entry.street.c_str(),
            entry.number.c_str(),
            entry.type,
            entry.note.c_str(),
            entry.mail.c_str(),
            entry.ID
    );
}

ContactsAddressTableRow ContactsAddressTable::GetByID(uint32_t id) {
    auto retQuery = db->Query("SELECT * FROM contact_address WHERE _id= %lu;", id);

    if ( (retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return ContactsAddressTableRow();
    }

    return ContactsAddressTableRow{(*retQuery)[0].GetUInt32(),  // ID
                                (*retQuery)[1].GetUInt32(),    // contactID
                                (*retQuery)[2].GetString(),    // country
                                (*retQuery)[3].GetString(),    // city
                                (*retQuery)[4].GetString(),    // street
                                (*retQuery)[5].GetString(),    // number
                                static_cast<ContactAddressType >((*retQuery)[6].GetUInt32()),    // type
                                (*retQuery)[7].GetString(),    // note
                                (*retQuery)[8].GetString(),    // mail
    };
}

std::vector<ContactsAddressTableRow> ContactsAddressTable::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto retQuery = db->Query("SELECT * from contact_address ORDER BY contact_id LIMIT %lu OFFSET %lu;",
                              limit,
                              offset);

    if ( (retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsAddressTableRow>();
    }

    std::vector<ContactsAddressTableRow> ret;

    do {
        ret.push_back(ContactsAddressTableRow{(*retQuery)[0].GetUInt32(),  // ID
                                              (*retQuery)[1].GetUInt32(),    // contactID
                                              (*retQuery)[2].GetString(),    // country
                                              (*retQuery)[3].GetString(),    // city
                                              (*retQuery)[4].GetString(),    // street
                                              (*retQuery)[5].GetString(),    // number
                                              static_cast<ContactAddressType >((*retQuery)[6].GetUInt32()),    // type
                                              (*retQuery)[7].GetString(),    // note
                                              (*retQuery)[8].GetString(),    // mail
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<ContactsAddressTableRow> ContactsAddressTable::GetLimitOffsetByFieldID(uint32_t offset, uint32_t limit,
                                                                                   const char *field, uint32_t id) {
    auto retQuery = db->Query("SELECT * from contact_address WHERE %s=%lu ORDER BY contact_id LIMIT %lu OFFSET %lu;",
                              field,
                              id,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsAddressTableRow>();
    }

    std::vector<ContactsAddressTableRow> ret;

    do {
        ret.push_back(ContactsAddressTableRow{(*retQuery)[0].GetUInt32(),  // ID
                                              (*retQuery)[1].GetUInt32(),    // contactID
                                              (*retQuery)[2].GetString(),    // country
                                              (*retQuery)[3].GetString(),    // city
                                              (*retQuery)[4].GetString(),    // street
                                              (*retQuery)[5].GetString(),    // number
                                              static_cast<ContactAddressType >((*retQuery)[6].GetUInt32()),    // type
                                              (*retQuery)[7].GetString(),    // note
                                              (*retQuery)[8].GetString(),    // mail
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t ContactsAddressTable::GetCount() {
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_address;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ContactsAddressTable::GetCountByFieldID(const char *field, uint32_t id) {
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_address WHERE %s=%lu;",field,id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}