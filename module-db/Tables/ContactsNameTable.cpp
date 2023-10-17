// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactsNameTable.hpp"
#include "Common/Types.hpp"
#include <Utils.hpp>

ContactsNameTable::ContactsNameTable(Database *db) : Table(db)
{}

ContactsNameTable::~ContactsNameTable()
{}

bool ContactsNameTable::create()
{
    return true;
}

bool ContactsNameTable::add(ContactsNameTableRow entry)
{
    return db->execute("insert or ignore into contact_name (contact_id, name_primary, name_alternative) "
                       "VALUES (" u32_c str_c str_ ");",
                       entry.contactID,
                       entry.namePrimary.c_str(),
                       entry.nameAlternative.c_str());
}

bool ContactsNameTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM contact_name where _id=" u32_ ";", id);
}

bool ContactsNameTable::update(ContactsNameTableRow entry)
{
    return db->execute("UPDATE contact_name SET contact_id=" u32_c "name_primary=" str_c "name_alternative=" str_
                       "WHERE _id=" u32_ ";",
                       entry.contactID,
                       entry.namePrimary.c_str(),
                       entry.nameAlternative.c_str(),
                       entry.ID);
}

ContactsNameTableRow ContactsNameTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM contact_name WHERE _id=" u32_ ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return ContactsNameTableRow();
    }

    return ContactsNameTableRow{
        {(*retQuery)[0].getUInt32()}, // ID
        (*retQuery)[1].getUInt32(),   // contactID
        (*retQuery)[2].getString(),   // namePrimary
        (*retQuery)[3].getString(),   // nameAlternative
    };
}

std::vector<ContactsNameTableRow> ContactsNameTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query(
        "SELECT * from contact_name ORDER BY name_alternative ASC LIMIT " u32_ " OFFSET " u32_ ";", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsNameTableRow>();
    }

    std::vector<ContactsNameTableRow> ret;

    do {
        ret.push_back(ContactsNameTableRow{
            {(*retQuery)[0].getUInt32()}, // ID
            (*retQuery)[1].getUInt32(),   // contactID
            (*retQuery)[2].getString(),   // namePrimary
            (*retQuery)[3].getString(),   // nameAlternative
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<ContactsNameTableRow> ContactsNameTable::getLimitOffsetByField(uint32_t offset,
                                                                           uint32_t limit,
                                                                           ContactNameTableFields field,
                                                                           const char *str)
{

    std::string fieldName;

    switch (field) {
    case ContactNameTableFields::NamePrimary:
        fieldName = "name_primary";
        break;
    case ContactNameTableFields::NameAlternative:
        fieldName = "name_alternative";
        break;
    default:
        return std::vector<ContactsNameTableRow>();
    }

    auto retQuery = db->query("SELECT * from contact_name WHERE %q=" str_ " ORDER BY name_alternative LIMIT " u32_
                              " OFFSET " u32_ ";",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsNameTableRow>();
    }

    std::vector<ContactsNameTableRow> ret;

    do {
        ret.push_back(ContactsNameTableRow{
            {(*retQuery)[0].getUInt32()}, // ID
            (*retQuery)[1].getUInt32(),   // contactID
            (*retQuery)[2].getString(),   // namePrimary
            (*retQuery)[3].getString(),   // nameAlternative
        });
    } while (retQuery->nextRow());

    return ret;
}

uint32_t ContactsNameTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_name;");

    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t ContactsNameTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_name WHERE %q=" u32_ ";", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

std::vector<ContactsNameTableRow> ContactsNameTable::GetByName(const char *primaryName, const char *alternativeName)
{

    auto retQuery =
        db->query("SELECT * from contact_name WHERE name_primary=" str_ " AND name_alternative=" str_ " ORDER BY "
                  "name_alternative LIMIT 1;",
                  primaryName,
                  alternativeName);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsNameTableRow>();
    }

    std::vector<ContactsNameTableRow> ret;

    do {
        ret.push_back(ContactsNameTableRow{
            {(*retQuery)[0].getUInt32()}, // ID
            (*retQuery)[1].getUInt32(),   // contactID
            (*retQuery)[2].getString(),   // namePrimary
            (*retQuery)[3].getString(),   // nameAlternative
        });
    } while (retQuery->nextRow());

    return ret;
}

std::size_t ContactsNameTable::GetCountByName(const std::string &name)
{
    if (name.empty()) {
        return count();
    }

    const auto names     = utils::split(name, " ");
    const auto namePart1 = names[0];
    const auto namePart2 = names.size() > 1 ? names[1] : "";

    std::unique_ptr<QueryResult> queryRet;

    if (!namePart1.empty() && !namePart2.empty()) {
        queryRet = db->query(
            "SELECT COUNT(*) FROM contact_name WHERE (name_primary like '%q%%' AND name_alternative like '%q%%') OR "
            "(name_primary like '%q%%' AND name_alternative like '%q%%');",
            namePart1.c_str(),
            namePart2.c_str(),
            namePart2.c_str(),
            namePart1.c_str());
    }
    else {
        queryRet = db->query(
            "SELECT COUNT(*) FROM contact_name WHERE name_primary like '%q%%' OR name_alternative like '%q%%';",
            namePart1.c_str(),
            namePart1.c_str());
    }

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}
