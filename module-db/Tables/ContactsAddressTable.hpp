#pragma once

#include "Common/Common.hpp"
#include "Database/Database.hpp"
#include "Table.hpp"
#include "Record.hpp"
#include "utf8/UTF8.hpp"
#include <string>

struct ContactsAddressTableRow
{
    uint32_t ID        = DB_ID_NONE;
    uint32_t contactID = 0;
    UTF8 addressLine1  = "";
    UTF8 addressLine2  = "";
    UTF8 note          = "";
    UTF8 mail          = "";
};

enum class ContactAddressTableFields
{
    Mail
};

class ContactsAddressTable : public Table<ContactsAddressTableRow, ContactAddressTableFields>
{
  public:
    ContactsAddressTable(Database *db);

    virtual ~ContactsAddressTable();

    bool Create() override final;

    bool Add(ContactsAddressTableRow entry) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(ContactsAddressTableRow entry) override final;

    ContactsAddressTableRow GetByID(uint32_t id) override final;

    std::vector<ContactsAddressTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsAddressTableRow> GetLimitOffsetByField(uint32_t offset,
                                                               uint32_t limit,
                                                               ContactAddressTableFields field,
                                                               const char *str) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS contact_address("
                                   "_id              INTEGER PRIMARY KEY,"
                                   "contact_id       INTEGER,"
                                   "address_line1    TEXT NOT NULL,"
                                   "address_line2    TEXT NOT NULL,"
                                   "note             TEXT NOT NULL,"
                                   "mail             TEXT NOT NULL,"
                                   "FOREIGN KEY(contact_id) REFERENCES contacts(_id)"
                                   ");";
};
