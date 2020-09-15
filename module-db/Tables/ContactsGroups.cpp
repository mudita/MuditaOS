#include "ContactsGroups.hpp"

const char *createTablesQuery = "CREATE TABLE IF NOT EXISTS contact_groups("
                                "  _id  INTEGER PRIMARY KEY,"
                                "  name TEXT NOT NULL UNIQUE"
                                ");"
                                "CREATE TABLE IF NOT EXISTS contact_match_groups("
                                "  _id        INTEGER PRIMARY KEY,"
                                "  group_id   INTEGER,"
                                "  contact_id INTEGER,"
                                "  FOREIGN KEY(group_id) REFERENCES contact_groups(_id)"
                                "      ON DELETE CASCADE,"
                                "  FOREIGN KEY(contact_id) REFERENCES contacts(_id)"
                                "      ON DELETE CASCADE,"
                                "  CONSTRAINT unique_group_contact"
                                "      UNIQUE (group_id,contact_id)"
                                ");";
const char *createIndices = "CREATE INDEX IF NOT EXISTS contact_match_group_index_on_group"
                            "  ON contact_match_groups(group_id);"
                            "CREATE INDEX IF NOT EXISTS contact_match_group_index_on_contact"
                            "  ON contact_match_groups(contact_id);";

const char *addSpecialGroups = "INSERT OR REPLACE INTO contact_groups "
                               " (_id, name)"
                               "VALUES"
                               " (1, 'Favourites'),"
                               " (2, 'ICE'),"
                               " (3, 'Blocked'),"
                               " (4, 'Temporary');";

namespace statements
{
    const char *countGroups = "SELECT COUNT(*) FROM contact_groups;";

    const char *favouritesId = "SELECT _id FROM contact_groups WHERE name = 'Favourites';";
    const char *iceId        = "SELECT _id FROM contact_groups WHERE name = 'ICE';";
    const char *blockedId    = "SELECT _id FROM contact_groups WHERE name = 'Blocked';";
    const char *temporaryId  = "SELECT _id FROM contact_groups WHERE name = 'Temporary';";

    const char *getId      = "SELECT _id FROM contact_groups WHERE name = '%q';";
    const char *getById    = "SELECT _id, name FROM contact_groups WHERE _id = %u;";
    const char *deleteById = "DELETE FROM contact_groups WHERE _id = %u;";

    const char *getAllLimtOfset = "SELECT _id, name FROM contact_groups ORDER BY _id LIMIT %lu OFFSET %lu;";

    const char *addGrup         = "INSERT INTO contact_groups (name) VALUES ('%q');";
    const char *updateGroupName = "UPDATE contact_groups SET name = '%q' WHERE _id = '%u';";
    const char *deleteGroup     = "DELETE FROM table_name WHERE _id = :id;";

    const char *addContactToGroup = "INSERT INTO contact_match_groups (contact_id, group_id)"
                                    "  VALUES(%u, %u)";

    const char *delContactFromGroup = "DELETE FROM contact_match_groups "
                                      "   WHERE "
                                      "      contact_id = %u "
                                      "      AND "
                                      "      group_id = %u;";

    const char *getGroupsForContact = "SELECT groups._id, groups.name "
                                      " FROM contact_groups as groups,"
                                      "     contact_match_groups as cmg, "
                                      "     contacts "
                                      " WHERE contacts._id = cmg.contact_id "
                                      "      AND groups._id = cmg.group_id "
                                      "      AND contacts._id = %u "
                                      " ORDER BY groups._id ASC;";

    const char *getContactsForGroup = "SELECT cmg.contact_id FROM contact_match_groups as cmg "
                                      " WHERE cmg.group_id = %u;";
} // namespace statements

ContactsGroupsTable::ContactsGroupsTable(Database *db) : Table(db)
{}

bool ContactsGroupsTable::create()
{
    if (db->execute(createTablesQuery)) {
        if (db->execute(createIndices)) {
            if (db->execute(addSpecialGroups)) {
                return true;
            }
        }
    }
    return false;
}

bool ContactsGroupsTable::add(ContactsGroupsTableRow entry)
{
    if (!entry.isValid() && (!entry.name.empty())) {
        return db->execute(statements::addGrup, entry.name.c_str());
    }
    return false;
}

bool ContactsGroupsTable::removeById(uint32_t id)
{
    if (id != DB_ID_NONE) {
        return db->execute(statements::deleteById, id);
    }
    return false;
}

bool ContactsGroupsTable::update(ContactsGroupsTableRow entry)
{
    if (entry.isValid() && (!entry.name.empty())) {
        return db->execute(statements::updateGroupName, entry.name.c_str(), entry.ID);
    }
    return false;
}

void ContactsGroupsTable::updateGroups(uint32_t contactId, std::set<ContactsGroupsTableRow> newGroups)
{
    auto currentGroups = getGroupsForContact(contactId);
    std::set<ContactsGroupsTableRow> groupsToRemove;
    for (auto group : currentGroups) {
        auto groupNode = newGroups.extract(group);
        if (groupNode.empty()) {
            groupsToRemove.insert(group);
        }
    }
    // 1. removing from groups
    for (auto group : groupsToRemove) {
        removeContactFromGroup(contactId, group.ID);
    }
    // 2. add to new groups
    for (auto group : newGroups) {
        addContactToGroup(contactId, group.ID);
    }
}

ContactsGroupsTableRow ContactsGroupsTable::getById(uint32_t id)
{
    if (id != DB_ID_NONE) {
        auto qureryResult = db->query(statements::getById, id);
        if (qureryResult->getRowCount() == 1) {
            return {(*qureryResult)[0].getUInt32(), (*qureryResult)[1].getString()};
        }
    }
    return ContactsGroupsTableRow();
}

std::vector<ContactsGroupsTableRow> ContactsGroupsTable::getAllRows()
{
    uint32_t countItems = count();
    return getLimitOffset(0, countItems);
}

std::vector<ContactsGroupsTableRow> ContactsGroupsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    if (limit == 0) {
        limit = count();
    }
    auto queryResult = db->query(statements::getAllLimtOfset, limit, offset);
    if (queryResult != nullptr && queryResult->getRowCount() > 0) {
        std::vector<ContactsGroupsTableRow> groups;
        groups.reserve(queryResult->getRowCount());
        do {
            groups.emplace_back(ContactsGroupsTableRow((*queryResult)[0].getUInt32(), (*queryResult)[1].getString()));
        } while (queryResult->nextRow());
        return groups;
    }
    return std::vector<ContactsGroupsTableRow>();
}

std::vector<ContactsGroupsTableRow> ContactsGroupsTable::getLimitOffsetByField(uint32_t /*offset*/,
                                                                               uint32_t /*limit*/,
                                                                               ContactsGroupsTableFields /*field*/,
                                                                               const char * /*str*/)
{
    return std::vector<ContactsGroupsTableRow>();
}

uint32_t ContactsGroupsTable::count()
{
    return getIdOrCount(statements::countGroups);
}

bool ContactsGroupsTable::removeByField(ContactsGroupsTableFields /*field*/, const char * /*str*/)
{
    return false;
}

uint32_t ContactsGroupsTable::countByFieldId(const char * /*field*/, uint32_t /*id*/)
{
    return 0;
}

uint32_t ContactsGroupsTable::favouritesId() const
{
    return getIdOrCount(statements::favouritesId);
}

uint32_t ContactsGroupsTable::iceId() const
{
    return getIdOrCount(statements::iceId);
}

uint32_t ContactsGroupsTable::blockedId() const
{
    return getIdOrCount(statements::blockedId);
}

uint32_t ContactsGroupsTable::temporaryId() const
{
    return getIdOrCount(statements::temporaryId);
}

uint32_t ContactsGroupsTable::getId(const std::string &name)
{
    if (!name.empty()) {
        auto queryRet = db->query(statements::getId, name.c_str());
        if (queryRet->getRowCount() != 0) {
            return (*queryRet)[0].getUInt32();
        }
    }
    return 0;
}

std::set<ContactsGroupsTableRow> ContactsGroupsTable::getGroupsForContact(uint32_t contactId)
{
    auto queryResult = db->query(statements::getGroupsForContact, contactId);
    if (queryResult->getRowCount() != 0) {
        std::set<ContactsGroupsTableRow> results;
        do {
            results.insert(ContactsGroupsTableRow((*queryResult)[0].getUInt32(), (*queryResult)[1].getString()));
        } while (queryResult->nextRow());
        return results;
    }
    return {};
}

bool ContactsGroupsTable::addContactToGroup(uint32_t contactId, uint32_t groupId)
{
    return db->execute(statements::addContactToGroup, contactId, groupId);
}

bool ContactsGroupsTable::removeContactFromGroup(uint32_t contactId, uint32_t groupId)
{
    return db->execute(statements::delContactFromGroup, contactId, groupId);
}

std::set<uint32_t> ContactsGroupsTable::getContactsForGroup(uint32_t groupId)
{
    (void)(&groupId);
    auto queryResults = db->query(statements::getContactsForGroup, groupId);
    if (queryResults->getRowCount() != 0) {
        std::set<uint32_t> contacts;
        do {
            contacts.insert((*queryResults)[0].getUInt32());
        } while (queryResults->nextRow());
        return contacts;
    }
    return {};
}

uint32_t ContactsGroupsTable::getIdOrCount(const char *queryString) const
{
    auto queryRet = db->query(queryString);
    if (queryRet->getRowCount() == 0) {
        return 0;
    }
    return (*queryRet)[0].getUInt32();
}

ContactsGroupsTableRow::ContactsGroupsTableRow(std::string name) : name(name)
{}

ContactsGroupsTableRow::ContactsGroupsTableRow(uint32_t id, std::string name) : name(name)
{
    ID = id;
}

ContactsGroupsTableRow::ContactsGroupsTableRow(uint32_t id)
{
    ID = id;
}
