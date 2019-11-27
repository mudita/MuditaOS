
/*
 * @file ContactsAddressTable.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_CONTACTSADDRESSTABLE_HPP
#define PUREPHONE_CONTACTSADDRESSTABLE_HPP

#include <string>
#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct ContactsAddressTableRow {
    uint32_t    ID;
    uint32_t    contactID;
    UTF8        country;
    UTF8        city;
    UTF8        street;
    UTF8        number;
    ContactAddressType type;
    UTF8        note;
    UTF8        mail;
};

enum class ContactAddressTableFields{
    Country,
    City,
    Street,
    Number,
    Mail
};

class ContactsAddressTable : public Table<ContactsAddressTableRow,ContactAddressTableFields> {
public:

    ContactsAddressTable(Database* db);

    virtual ~ContactsAddressTable();

    bool Create() override final;

    bool Add(ContactsAddressTableRow entry) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(ContactsAddressTableRow entry) override final;

    ContactsAddressTableRow GetByID(uint32_t id) override final;

    std::vector<ContactsAddressTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsAddressTableRow>
    GetLimitOffsetByField(uint32_t offset, uint32_t limit, ContactAddressTableFields field,const char* str) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

private:

    const char *createTableQuery =
            "CREATE TABLE IF NOT EXISTS contact_address("
            "_id              INTEGER PRIMARY KEY,"
            "contact_id       INTEGER,"
            "country          TEXT NOT NULL,"
            "city             TEXT NOT NULL,"
            "street           TEXT NOT NULL,"
            "number           TEXT NOT NULL,"
            "type             INTEGER,"
            "note             TEXT NOT NULL,"
            "mail             TEXT NOT NULL,"
            "FOREIGN KEY(contact_id) REFERENCES contacts(_id)"
            ");";
};


#endif //PUREPHONE_CONTACTSADDRESSTABLE_HPP
