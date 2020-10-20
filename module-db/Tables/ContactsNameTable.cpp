// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * @file ContactsName.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "ContactsNameTable.hpp"

ContactsNameTable::ContactsNameTable(Database *db) : Table(db)
{}

ContactsNameTable::~ContactsNameTable()
{}

bool ContactsNameTable::create()
{
    return db->execute(createTableQuery);
}

bool ContactsNameTable::add(ContactsNameTableRow entry)
{
    return db->execute("insert or ignore into contact_name (contact_id, name_primary, name_alternative) "
                       "VALUES (%lu, '%q', '%q');",
                       entry.contactID,
                       entry.namePrimary.c_str(),
                       entry.nameAlternative.c_str());
}

bool ContactsNameTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM contact_name where _id = %lu;", id);
}

bool ContactsNameTable::update(ContactsNameTableRow entry)
{
    return db->execute("UPDATE contact_name SET contact_id = %lu, name_primary = '%q', name_alternative = '%q' "
                       "WHERE _id = %lu;",
                       entry.contactID,
                       entry.namePrimary.c_str(),
                       entry.nameAlternative.c_str(),
                       entry.ID);
}

ContactsNameTableRow ContactsNameTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM contact_name WHERE _id= %lu;", id);

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
    auto retQuery =
        db->query("SELECT * from contact_name ORDER BY name_alternative ASC LIMIT %lu OFFSET %lu;", limit, offset);

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
    case ContactNameTableFields ::NamePrimary:
        fieldName = "name_primary";
        break;
    case ContactNameTableFields ::NameAlternative:
        fieldName = "name_alternative";
        break;
    default:
        return std::vector<ContactsNameTableRow>();
    }

    auto retQuery =
        db->query("SELECT * from contact_name WHERE %q='%q' ORDER BY name_alternative LIMIT %lu OFFSET %lu;",
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

    if (queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t ContactsNameTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_name WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

std::vector<ContactsNameTableRow> ContactsNameTable::GetByName(const char *primaryName, const char *alternativeName)
{

    auto retQuery = db->query("SELECT * from contact_name WHERE name_primary='%q' AND name_alternative='%q' ORDER BY "
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
    auto queryRet =
        db->query("SELECT COUNT(*) FROM contact_name WHERE name_primary like '%s%%' OR name_alternative like '%s%%';",
                  name.c_str(),
                  name.c_str());

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}
