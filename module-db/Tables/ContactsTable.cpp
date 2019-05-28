
/*
 * @file ContactsTable.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ContactsTable.hpp"


ContactsTable::ContactsTable(Database* db):db(db){
}

ContactsTable::~ContactsTable() {

}

bool ContactsTable::Create() {
    return db->Execute(createTableQuery);
}

bool ContactsTable::Add(ContactsTableRow entry) {
    return db->Execute(
            "insert into contacts (name_id, numbers_id, ring_id, address_ids, type, whitelist, blacklist, favourites, speeddial ) VALUES (%lu, '%s', %lu, '%s', %lu, %lu, %lu, %lu, %lu);",
            entry.nameID,
            entry.numbersID.c_str(),
            entry.ringID,
            entry.addressIDs.c_str(),
            entry.type,
            entry.isOnWhitelist,
            entry.isOnBlacklist,
            entry.isOnFavourites,
            entry.speedDial
    );
}

bool ContactsTable::RemoveByID(uint32_t id) {
    return db->Execute("DELETE FROM contacts where _id = %u;", id);
}

bool ContactsTable::Update(ContactsTableRow entry) {
    return db->Execute(
            "UPDATE contacts SET name_id = %lu, numbers_id = '%s' ,ring_id = %lu, address_ids = '%s', type = %lu, whitelist = %lu, blacklist = %lu, favourites = %lu, speeddial = %lu WHERE _id=%lu;",
            entry.nameID,
            entry.numbersID.c_str(),
            entry.ringID,
            entry.addressIDs.c_str(),
            entry.type,
            entry.isOnWhitelist,
            entry.isOnBlacklist,
            entry.isOnFavourites,
            entry.speedDial,
            entry.ID
    );
}

ContactsTableRow ContactsTable::GetByID(uint32_t id) {
    auto retQuery = db->Query("SELECT * FROM contacts WHERE _id= %lu;", id);

    if (retQuery->GetRowCount() == 0) {
        return ContactsTableRow();
    }

    return ContactsTableRow{(*retQuery)[0].GetUInt32(),  // ID
                       (*retQuery)[1].GetUInt32(),    // nameID
                       (*retQuery)[2].GetString(),    // numbersID
                       (*retQuery)[3].GetUInt32(),    // ringID
                       (*retQuery)[4].GetString(),    // addressID
                       static_cast<ContactType >((*retQuery)[5].GetUInt32()),    // type
                       (*retQuery)[6].GetBool(),    // is on whitelist
                       (*retQuery)[7].GetBool(),      // is on blacklist
                       (*retQuery)[8].GetBool(),      // is on favourites
                       (*retQuery)[9].GetUInt32(),      // speed dial key
    };
}

std::vector<ContactsTableRow> ContactsTable::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto retQuery = db->Query("SELECT * from contacts ORDER BY name_id LIMIT %lu OFFSET %lu;",
                              limit,
                              offset);

    if (retQuery->GetRowCount() == 0) {
        return std::vector<ContactsTableRow>();
    }

    std::vector<ContactsTableRow> ret;

    do {
        ret.push_back(ContactsTableRow{(*retQuery)[0].GetUInt32(),  // ID
                                       (*retQuery)[1].GetUInt32(),    // nameID
                                       (*retQuery)[2].GetString(),    // numbersID
                                       (*retQuery)[3].GetUInt32(),    // ringID
                                       (*retQuery)[4].GetString(),    // addressID
                                       static_cast<ContactType >((*retQuery)[5].GetUInt32()),    // type
                                       (*retQuery)[6].GetBool(),    // is on whitelist
                                       (*retQuery)[7].GetBool(),      // is on blacklist
                                       (*retQuery)[8].GetBool(),      // is on favourites
                                       (*retQuery)[9].GetUInt32(),      // speed dial key
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<ContactsTableRow> ContactsTable::GetLimitOffsetByFieldID(uint32_t offset, uint32_t limit, const char *field,
                                                                     uint32_t id) {
    auto retQuery = db->Query("SELECT * from contacts WHERE %s=%lu ORDER BY name_id LIMIT %lu OFFSET %lu;",
                              field,
                              id,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsTableRow>();
    }

    std::vector<ContactsTableRow> ret;

    do {
        ret.push_back(ContactsTableRow{(*retQuery)[0].GetUInt32(),  // ID
                                       (*retQuery)[1].GetUInt32(),    // nameID
                                       (*retQuery)[2].GetString(),    // numbersID
                                       (*retQuery)[3].GetUInt32(),    // ringID
                                       (*retQuery)[4].GetString(),    // addressID
                                       static_cast<ContactType >((*retQuery)[5].GetUInt32()),    // type
                                       (*retQuery)[6].GetBool(),    // is on whitelist
                                       (*retQuery)[7].GetBool(),      // is on blacklist
                                       (*retQuery)[8].GetBool(),      // is on favourites
                                       (*retQuery)[9].GetUInt32(),      // speed dial key
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t ContactsTable::GetCount() {
    auto queryRet = db->Query("SELECT COUNT(*) FROM contacts;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ContactsTable::GetCountByFieldID(const char *field, uint32_t id) {
    auto queryRet = db->Query("SELECT COUNT(*) FROM contacts WHERE %s=%lu;",field,id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}