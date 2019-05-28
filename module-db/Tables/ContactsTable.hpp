
/*
 * @file ContactsTable.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_CONTACTSTABLE_HPP
#define PUREPHONE_CONTACTSTABLE_HPP


#include <string>
#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct ContactsTableRow {
    uint32_t ID;
    uint32_t nameID;
    std::string numbersID;
    uint32_t ringID;
    std::string addressIDs;
    ContactType type;
    bool isOnWhitelist;
    bool isOnBlacklist;
    bool isOnFavourites;
    uint32_t speedDial;
};


class ContactsTable : public Table<ContactsTableRow> {
public:

    ContactsTable(Database* db);

    ~ContactsTable();

    bool Create() override final;

    bool Add(ContactsTableRow entry) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(ContactsTableRow entry) override final;

    ContactsTableRow GetByID(uint32_t id) override final;

    std::vector<ContactsTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsTableRow>
    GetLimitOffsetByFieldID(uint32_t offset, uint32_t limit, const char *field, uint32_t id) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

private:

    const char *createTableQuery =
            "CREATE TABLE IF NOT EXISTS contacts("
            "_id INTEGER PRIMARY KEY,"
            "name_id INTEGER,"
            "numbers_id TEXT NOT NULL,"
            "ring_id INTEGER,"
            "address_ids TEXT NOT NULL,"
            "type INTEGER,"
            "whitelist        INTEGER,"
            "blacklist        INTEGER,"
            "favourites       INTEGER,"
            "speeddial        INTEGER,"
            "FOREIGN KEY(name_id) REFERENCES contact_names(_id) FOREIGN KEY(ring_id) REFERENCES contact_ring(_id)"
            ");";

    Database* db;

};


#endif //PUREPHONE_CONTACTSTABLE_HPP
