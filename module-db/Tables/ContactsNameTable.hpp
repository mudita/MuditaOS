// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Common.hpp"
#include "Database/Database.hpp"
#include "Table.hpp"
#include "Record.hpp"
#include "utf8/UTF8.hpp"
#include <string>

struct ContactsNameTableRow : public Record
{
    uint32_t contactID   = 0;
    UTF8 namePrimary     = "";
    UTF8 nameAlternative = "";
};

enum class ContactNameTableFields
{
    NamePrimary,
    NameAlternative,
};

class ContactsNameTable : public Table<ContactsNameTableRow, ContactNameTableFields>
{
  public:
    ContactsNameTable(Database *db);

    virtual ~ContactsNameTable();

    bool create() override final;

    bool add(ContactsNameTableRow entry) override final;

    bool removeById(uint32_t id) override final;

    bool update(ContactsNameTableRow entry) override final;

    ContactsNameTableRow getById(uint32_t id) override final;

    std::vector<ContactsNameTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsNameTableRow> getLimitOffsetByField(uint32_t offset,
                                                            uint32_t limit,
                                                            ContactNameTableFields field,
                                                            const char *str) override final;

    uint32_t count() override final;

    uint32_t countByFieldId(const char *field, uint32_t id) override final;

    std::vector<ContactsNameTableRow> GetByName(const char *primaryName, const char *alternativeName);

    std::size_t GetCountByName(const std::string &name);

  private:
};
