
/*
 * @file ContactsNumberTable.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Common/Common.hpp"
#include "Database/Database.hpp"
#include "Table.hpp"
#include "utf8/UTF8.hpp"
#include <string>

struct ContactsNumberTableRow
{
    uint32_t ID            = 0;
    uint32_t contactID     = 0;
    std::string numberUser = "";
    std::string numbere164 = "";
    ContactNumberType type = ContactNumberType::OTHER;
};

enum class ContactNumberTableFields
{
    NumberUser,
    NumberE164

};

class ContactsNumberTable : public Table<ContactsNumberTableRow, ContactNumberTableFields>
{
  public:
    ContactsNumberTable(Database *db);

    virtual ~ContactsNumberTable();

    bool Create() override final;

    bool Add(ContactsNumberTableRow entry) override final;

    bool RemoveByID(uint32_t id) override final;

    bool Update(ContactsNumberTableRow entry) override final;

    ContactsNumberTableRow GetByID(uint32_t id) override final;

    std::vector<ContactsNumberTableRow> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsNumberTableRow> GetLimitOffsetByField(uint32_t offset,
                                                              uint32_t limit,
                                                              ContactNumberTableFields field,
                                                              const char *str) override final;

    uint32_t GetCount() override final;

    uint32_t GetCountByFieldID(const char *field, uint32_t id) override final;

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS contact_number("
                                   "_id              INTEGER PRIMARY KEY,"
                                   "contact_id       INTEGER,"
                                   "number_user      TEXT NOT NULL,"
                                   "number_e164      TEXT NOT NULL,"
                                   "type             INTEGER,"
                                   "FOREIGN KEY(contact_id) REFERENCES contacts(_id)"
                                   ");";
};
