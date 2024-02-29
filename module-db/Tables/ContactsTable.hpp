// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common/Common.hpp"
#include "Common/Logging.hpp"
#include "Record.hpp"
#include "Table.hpp"
#include "utf8/UTF8.hpp"
#include <module-apps/application-phonebook/data/ContactsMap.hpp>

#include <string>
#include <vector>
#include <map>

struct ContactsTableRow : public Record
{
    std::uint32_t nameID    = DB_ID_NONE;
    std::string numbersID = "";
    std::uint32_t ringID    = DB_ID_NONE;
    std::uint32_t addressID = DB_ID_NONE;
    std::string speedDial = "";
    UTF8 namePrimary      = "";
    UTF8 nameAlternative  = "";
};

enum class ContactTableFields
{
    SpeedDial
};

class ContactsTable : public Table<ContactsTableRow, ContactTableFields>
{
  public:
    enum class MatchType
    {
        Name,
        TextNumber,
        Group,
        None
    };

    ContactsTable(Database *db);
    virtual ~ContactsTable();

    bool create() override final;
    bool add(ContactsTableRow entry) override final;
    bool removeById(std::uint32_t id) override final;
    bool update(ContactsTableRow entry) override final;

    ContactsTableRow getById(std::uint32_t id) override final;
    ContactsTableRow getByIdWithTemporary(std::uint32_t id);

    bool BlockByID(std::uint32_t id, bool shouldBeBlocked);

    std::vector<ContactsTableRow> Search(const std::string &primaryName,
                                         const std::string &alternativeName,
                                         const std::string &number);

    std::vector<ContactsTableRow> getLimitOffset(std::uint32_t offset, std::uint32_t limit) override final;
    std::vector<ContactsTableRow> getLimitOffsetByField(std::uint32_t offset,
                                                        std::uint32_t limit,
                                                        ContactTableFields field,
                                                        const char *str) override final;

    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;

    std::vector<std::uint32_t> GetIDsSortedByField(MatchType matchType,
                                                   const std::string &text,
                                                   std::uint32_t groupId,
                                                   std::uint32_t limit  = 0,
                                                   std::uint32_t offset = 0);
    std::vector<std::uint32_t> GetIDsSortedByName(std::uint32_t limit = 0, std::uint32_t offset = 0);

    ContactsMapData GetPosOfFirstLetters();
    std::string GetSortedByNameQueryString(ContactQuerySection section);

  private:
    ContactsTableRow getByIdCommon(std::unique_ptr<QueryResult> retQuery);
};
