
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
    uint32_t    favourite;
};

enum class ContactNameTableFields{
    NamePrimary,
    NameAlternative,
	Favourite
};

class ContactsNameTable : public Table<ContactsNameTableRow,ContactNameTableFields> {
public:

    ContactsNameTable(Database* db);

    virtual ~ContactsNameTable();

    bool Create() override final;

    bool Add(ContactsNameTableRow entry) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(ContactsNameTableRow entry) override final;

    ContactsNameTableRow GetByID(uint32_t id) override final;

    std::vector<ContactsNameTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsNameTableRow>
    GetLimitOffsetByField(uint32_t offset, uint32_t limit, ContactNameTableFields field,const char* str) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

    std::vector<ContactsNameTableRow>
    GetByName( const char* primaryName, const char* alternativeName );

private:

    const char *createTableQuery =
            "CREATE TABLE IF NOT EXISTS contact_name("
            "_id              INTEGER PRIMARY KEY,"
            "contact_id       INTEGER,"
            "name_primary     TEXT NOT NULL,"
            "name_alternative TEXT NOT NULL,"
    		"favourite        INTEGER DEFAULT 0,"
            "FOREIGN KEY(contact_id) REFERENCES contacts(_id)"
            ");";
};


#endif //PUREPHONE_CONTACTSNAME_HPP
