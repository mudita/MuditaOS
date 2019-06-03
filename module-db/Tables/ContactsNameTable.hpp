
/*
 * @file ContactsName.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_CONTACTSNAME_HPP
#define PUREPHONE_CONTACTSNAME_HPP

#include <string>
#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct ContactsNameTableRow {
    uint32_t    ID;
    uint32_t    contactID;
    UTF8        namePrimary;
    UTF8        nameAlternative;
};

class ContactsNameTable : public Table<ContactsNameTableRow> {
public:

    ContactsNameTable(Database* db);

    ~ContactsNameTable();

    bool Create() override final;

    bool Add(ContactsNameTableRow entry) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(ContactsNameTableRow entry) override final;

    ContactsNameTableRow GetByID(uint32_t id) override final;

    std::vector<ContactsNameTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsNameTableRow>
    GetLimitOffsetByFieldID(uint32_t offset, uint32_t limit, const char *field, uint32_t id) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

private:

    const char *createTableQuery =
            "CREATE TABLE IF NOT EXISTS contact_name("
            "_id              INTEGER PRIMARY KEY,"
            "contact_id       INTEGER,"
            "name_primary     TEXT NOT NULL,"
            "name_alternative TEXT NOT NULL,"
            "FOREIGN KEY(contact_id) REFERENCES contacts(_id)"
            ");";

    Database* db;
};


#endif //PUREPHONE_CONTACTSNAME_HPP
