// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Common.hpp"
#include "Database/Database.hpp"
#include "Table.hpp"
#include "utf8/UTF8.hpp"
#include <string>
#include "Record.hpp"

struct ContactsRingtonesTableRow : public Record
{
    uint32_t contactID = DB_ID_NONE;
    UTF8 assetPath     = "";
    ContactsRingtonesTableRow(uint32_t ID = DB_ID_NONE, uint32_t contactID = 0, UTF8 assetPath = "")
        : contactID(contactID), assetPath(assetPath)
    {
        this->ID = ID;
    }
    ContactsRingtonesTableRow(uint32_t contactID, UTF8 assetPath) : contactID(contactID), assetPath(assetPath)
    {}
};

enum class ContactRingtonesTableFields
{
    AssetPath
};

class ContactsRingtonesTable : public Table<ContactsRingtonesTableRow, ContactRingtonesTableFields>
{
  public:
    ContactsRingtonesTable(Database *db);

    virtual ~ContactsRingtonesTable();

    bool create() override final;

    bool add(ContactsRingtonesTableRow entry) override final;

    bool removeById(uint32_t id) override final;

    bool update(ContactsRingtonesTableRow entry) override final;

    ContactsRingtonesTableRow getById(uint32_t id) override final;

    std::vector<ContactsRingtonesTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::vector<ContactsRingtonesTableRow> getLimitOffsetByField(uint32_t offset,
                                                                 uint32_t limit,
                                                                 ContactRingtonesTableFields field,
                                                                 const char *str) override final;

    uint32_t count() override final;

    uint32_t countByFieldId(const char *field, uint32_t id) override final;

  private:
};
