// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"
#include "Common/Common.hpp"
#include "Database/Database.hpp"
#include "Table.hpp"
#include "utf8/UTF8.hpp"
#include <string>

struct ContactsNumberTableRow : public Record
{
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

    bool create() override final;

    bool add(ContactsNumberTableRow entry) override final;

    bool removeById(uint32_t id) override final;

    bool update(ContactsNumberTableRow entry) override final;

    ContactsNumberTableRow getById(uint32_t id) override final;

    std::vector<ContactsNumberTableRow> getByContactId(uint32_t id);

    std::vector<ContactsNumberTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsNumberTableRow> getLimitOffsetByField(uint32_t offset,
                                                              uint32_t limit,
                                                              ContactNumberTableFields field,
                                                              const char *str) override final;

    uint32_t count() override final;

    uint32_t countByFieldId(const char *field, uint32_t id) override final;

  private:
};
