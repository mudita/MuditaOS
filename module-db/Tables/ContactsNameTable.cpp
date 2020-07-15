
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
    return db->execute("insert or ignore into contact_name (contact_id, name_primary, name_alternative, favourite) "
                       "VALUES (%lu, '%q', '%q', '%lu');",
                       entry.contactID,
                       entry.namePrimary.c_str(),
                       entry.nameAlternative.c_str(),
                       entry.favourite);
}

bool ContactsNameTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM contact_name where _id = %u;", id);
}

bool ContactsNameTable::update(ContactsNameTableRow entry)
{
    return db->execute("UPDATE contact_name SET contact_id = %lu, name_primary = '%q', name_alternative = '%q', "
                       "favourite = '%lu' WHERE _id = %lu;",
                       entry.contactID,
                       entry.namePrimary.c_str(),
                       entry.nameAlternative.c_str(),
                       entry.favourite,
                       entry.ID);
}

ContactsNameTableRow ContactsNameTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM contact_name WHERE _id= %lu;", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return ContactsNameTableRow();
    }

    return ContactsNameTableRow{
        (*retQuery)[0].GetUInt32(), // ID
        (*retQuery)[1].GetUInt32(), // contactID
        (*retQuery)[2].GetString(), // namePrimary
        (*retQuery)[3].GetString(), // nameAlternative
        (*retQuery)[4].GetUInt32(), // favourite
    };
}

std::vector<ContactsNameTableRow> ContactsNameTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query(
        "SELECT * from contact_name ORDER BY favourite DESC, name_alternative LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsNameTableRow>();
    }

    std::vector<ContactsNameTableRow> ret;

    do {
        ret.push_back(ContactsNameTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // contactID
            (*retQuery)[2].GetString(), // namePrimary
            (*retQuery)[3].GetString(), // nameAlternative
            (*retQuery)[4].GetUInt32(), // favourite
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
    case ContactNameTableFields ::Favourite:
        fieldName = "favourite";
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
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // contactID
            (*retQuery)[2].GetString(), // namePrimary
            (*retQuery)[3].GetString(), // nameAlternative
            (*retQuery)[4].GetUInt32(), // favourite
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

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ContactsNameTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_name WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
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
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // contactID
            (*retQuery)[2].GetString(), // namePrimary
            (*retQuery)[3].GetString(), // nameAlternative
            (*retQuery)[4].GetUInt32(), // favourite
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<std::uint32_t> ContactsNameTable::GetIDsByName(const std::string &name,
                                                           std::uint32_t limit,
                                                           std::uint32_t offset)
{
    std::vector<std::uint32_t> ids;

    std::string query = "SELECT contact_id FROM contact_name";
    if (!name.empty()) {
        query += " WHERE name_primary || ' ' || name_alternative LIKE '" + name + "%%'";
        query += " OR name_alternative || ' ' || name_primary LIKE '" + name + "%%'";
    }
    query += " ORDER BY favourite DESC, name_alternative || ' ' || name_primary";

    if (limit > 0) {
        query += " LIMIT " + std::to_string(limit);
        query += " OFFSET " + std::to_string(offset);
    }

    query += " COLLATE NOCASE;";

    auto queryRet = db->query(query.c_str());
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return ids;
    }

    do {
        ids.push_back((*queryRet)[0].GetUInt32());
    } while (queryRet->nextRow());

    return ids;
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

    return uint32_t{(*queryRet)[0].GetUInt32()};
}
