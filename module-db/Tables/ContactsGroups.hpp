// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Database/Database.hpp>
#include "Record.hpp"
#include "Table.hpp"

#include <set>
#include <string>

struct ContactsGroupsTableRow : public Record
{
    ContactsGroupsTableRow() = default;
    ContactsGroupsTableRow(std::string name);
    ContactsGroupsTableRow(uint32_t id, std::string name);
    ContactsGroupsTableRow(uint32_t id);
    bool operator==(const ContactsGroupsTableRow &other) const
    {
        return this->ID == other.ID;
    }
    bool operator!=(const ContactsGroupsTableRow &other) const
    {
        return !this->operator==(other);
    }
    bool operator<(const ContactsGroupsTableRow &other) const
    {
        return this->ID < other.ID;
    }
    bool operator>(const ContactsGroupsTableRow &other) const
    {
        return this->ID > other.ID;
    }
    std::string name;
};

enum class ContactsGroupsTableFields
{
    GroupID,
    Name
};

struct ContactsMatchGroupsTableRow : public Record
{
    uint32_t groupID   = DB_ID_NONE;
    uint32_t contactID = DB_ID_NONE;
};

enum class ContacsMatchGroupsTableFields
{
    MatchID,
    GroupID,
    ContactID,
};

class ContactsGroupsTable : public Table<ContactsGroupsTableRow, ContactsGroupsTableFields>
{
  public:
    ContactsGroupsTable(Database *db);
    virtual ~ContactsGroupsTable() override = default;

    bool create() override final;

    /** Adds new group.
     * ID field have to be 0
     */
    bool add(ContactsGroupsTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool update(ContactsGroupsTableRow entry) override final;
    void updateGroups(uint32_t contactId, std::set<ContactsGroupsTableRow> newGroups);
    ContactsGroupsTableRow getById(uint32_t id) override final;
    std::vector<ContactsGroupsTableRow> getAllRows();
    std::vector<ContactsGroupsTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override;
    std::vector<ContactsGroupsTableRow> getLimitOffsetByField(uint32_t offset,
                                                              uint32_t limit,
                                                              ContactsGroupsTableFields field,
                                                              const char *str) override;
    bool removeByField(ContactsGroupsTableFields field, const char *str) override;
    uint32_t count() override;
    uint32_t countByFieldId(const char *field, uint32_t id) override;
    [[nodiscard]] uint32_t favouritesId() const;
    [[nodiscard]] uint32_t iceId() const;
    [[nodiscard]] uint32_t blockedId() const;
    [[nodiscard]] uint32_t temporaryId() const;
    /** Returns id of the grup
     */
    uint32_t getId(const std::string &name);

    std::set<ContactsGroupsTableRow> getGroupsForContact(uint32_t contactId);
    bool addContactToGroup(uint32_t contactId, uint32_t groupId);
    bool removeContactFromGroup(uint32_t contactId, uint32_t groupId);

    std::set<uint32_t> getContactsForGroup(uint32_t groupId);

  private:
    uint32_t getIdOrCount(const char *queryString) const;
};
