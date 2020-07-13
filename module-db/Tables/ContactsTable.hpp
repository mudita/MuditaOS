#pragma once

#include "Common/Common.hpp"
#include "Record.hpp"
#include "Table.hpp"
#include "utf8/UTF8.hpp"

#include <string>
#include <vector>

struct ContactsTableRow
{
    uint32_t ID           = DB_ID_NONE;
    uint32_t nameID       = DB_ID_NONE;
    std::string numbersID = "";
    uint32_t ringID       = DB_ID_NONE;
    uint32_t addressID    = DB_ID_NONE;
    ContactType type      = ContactType::TEMPORARY;
    bool isOnWhitelist    = false;
    bool isOnBlacklist    = false;
    bool isOnFavourites   = false;
    std::string speedDial = "";
    UTF8 namePrimary      = "";
    UTF8 nameAlternative  = "";
};

enum class ContactTableFields
{
    SpeedDial,
    Favourite
};

class ContactsTable : public Table<ContactsTableRow, ContactTableFields>
{
  public:
    ContactsTable(Database *db);

    virtual ~ContactsTable();

    bool Create() override final;

    bool Add(ContactsTableRow entry) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(ContactsTableRow entry) override final;

    ContactsTableRow GetByID(uint32_t id) override final;

    bool BlockByID(uint32_t id, bool shouldBeBlocked);

    std::vector<ContactsTableRow> Search(const std::string primaryName,
                                         const std::string alternativeName,
                                         const std::string number);

    std::vector<ContactsTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsTableRow> GetLimitOffsetByField(uint32_t offset,
                                                        uint32_t limit,
                                                        ContactTableFields field,
                                                        const char *str) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

    std::vector<std::uint32_t> GetIDsByTextNumber(const std::string &filter,
                                                  std::size_t limit  = 0,
                                                  std::size_t offset = 0);

  private:
    const char *createTableQuery =
        "CREATE TABLE IF NOT EXISTS contacts("
        "_id              INTEGER PRIMARY KEY,"
        "name_id          INTEGER,"
        "numbers_id       TEXT NOT NULL,"
        "ring_id          INTEGER,"
        "address_id       INTEGER,"
        "type             INTEGER,"
        "whitelist        INTEGER,"
        "blacklist        INTEGER,"
        "favourites       INTEGER,"
        "speeddial        TEXT NOT NULL,"
        "FOREIGN KEY(name_id) REFERENCES contact_name(_id) FOREIGN KEY(ring_id) REFERENCES contact_ringtones(_id)"
        ");";
};
