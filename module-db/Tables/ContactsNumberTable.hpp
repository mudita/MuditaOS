
/*
 * @file ContactsNumberTable.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_CONTACTSNUMBERTABLE_HPP
#define PUREPHONE_CONTACTSNUMBERTABLE_HPP

#include <string>
#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct ContactsNumberTableRow {
    uint32_t    ID;
    uint32_t    contactID;
    std::string        numberUser;
    std::string        numbere164;
    ContactNumberType   type;
};

class ContactsNumberTable : public Table<ContactsNumberTableRow> {
public:

    ContactsNumberTable(Database* db);

    ~ContactsNumberTable();

    bool Create() override final;

    bool Add(ContactsNumberTableRow entry) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(ContactsNumberTableRow entry) override final;

    ContactsNumberTableRow GetByID(uint32_t id) override final;

    std::vector<ContactsNumberTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsNumberTableRow>
    GetLimitOffsetByFieldID(uint32_t offset, uint32_t limit, const char *field, uint32_t id) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

private:

    const char *createTableQuery =
            "CREATE TABLE IF NOT EXISTS contact_number("
            "_id              INTEGER PRIMARY KEY,"
            "contact_id       INTEGER,"
            "number_user      TEXT NOT NULL,"
            "number_e164      TEXT NOT NULL,"
            "type             INTEGER,"
            "FOREIGN KEY(contact_id) REFERENCES contacts(_id)"
            ");";

    Database* db;
};


#endif //PUREPHONE_CONTACTSNUMBERTABLE_HPP
