
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

bool ContactsNameTable::Create()
{
    return db->Execute(createTableQuery);
}

bool ContactsNameTable::Add(ContactsNameTableRow entry)
{
    return db->Execute("insert or ignore into contact_name (contact_id, name_primary, name_alternative, favourite) "
                       "VALUES (%lu, '%q', '%q', '%lu');",
                       entry.contactID,
                       entry.namePrimary.c_str(),
                       entry.nameAlternative.c_str(),
                       entry.favourite);
}

bool ContactsNameTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM contact_name where _id = %u;", id);
}

bool ContactsNameTable::Update(ContactsNameTableRow entry)
{
    return db->Execute("UPDATE contact_name SET contact_id = %lu, name_primary = '%q', name_alternative = '%q', "
                       "favourite = '%lu' WHERE _id = %lu;",
                       entry.contactID,
                       entry.namePrimary.c_str(),
                       entry.nameAlternative.c_str(),
                       entry.favourite,
                       entry.ID);
}

ContactsNameTableRow ContactsNameTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM contact_name WHERE _id= %lu;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
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

std::vector<ContactsNameTableRow> ContactsNameTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery =
        db->Query("SELECT * from contact_name ORDER BY name_alternative LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
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
    } while (retQuery->NextRow());

    return ret;
}

std::vector<ContactsNameTableRow> ContactsNameTable::GetLimitOffsetByField(uint32_t offset,
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
        db->Query("SELECT * from contact_name WHERE %q='%q' ORDER BY name_alternative LIMIT %lu OFFSET %lu;",
                  fieldName.c_str(),
                  str,
                  limit,
                  offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
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
    } while (retQuery->NextRow());

    return ret;
}

uint32_t ContactsNameTable::GetCount()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_name;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ContactsNameTable::GetCountByFieldID(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_name WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

std::vector<ContactsNameTableRow> ContactsNameTable::GetByName(const char *primaryName, const char *alternativeName)
{

    auto retQuery = db->Query("SELECT * from contact_name WHERE name_primary='%q' AND name_alternative='%q' ORDER BY "
                              "name_alternative LIMIT 1;",
                              primaryName,
                              alternativeName);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
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
    } while (retQuery->NextRow());

    return ret;
}
