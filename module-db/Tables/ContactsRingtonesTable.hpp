
/*
 * @file ContactsRingtonesTable.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_CONTACTSRINGTONESTABLE_HPP
#define PUREPHONE_CONTACTSRINGTONESTABLE_HPP

#include <string>
#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct ContactsRingtonesTableRow {
    uint32_t    ID;
    uint32_t    contactID;
    UTF8        assetPath;
};

class ContactsRingtonesTable : public Table<ContactsRingtonesTableRow> {
public:

    ContactsRingtonesTable(Database* db);

    ~ContactsRingtonesTable();

    bool Create() override final;

    bool Add(ContactsRingtonesTableRow entry) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(ContactsRingtonesTableRow entry) override final;

    ContactsRingtonesTableRow GetByID(uint32_t id) override final;

    std::vector<ContactsRingtonesTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsRingtonesTableRow>
    GetLimitOffsetByFieldID(uint32_t offset, uint32_t limit, const char *field, uint32_t id) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

private:

    const char *createTableQuery =
            "CREATE TABLE IF NOT EXISTS contact_ringtones("
            "_id              INTEGER PRIMARY KEY,"
            "contact_id       INTEGER,"
            "asset_path       TEXT NOT NULL,"
            "FOREIGN KEY(contact_id) REFERENCES contacts(_id)"
            ");";

    Database* db;
};


#endif //PUREPHONE_CONTACTSRINGTONESTABLE_HPP
