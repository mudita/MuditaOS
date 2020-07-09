#include "ContactsTable.hpp"
#include <log/log.hpp>

ContactsTable::ContactsTable(Database *db) : Table(db)
{}

ContactsTable::~ContactsTable()
{}

bool ContactsTable::Create()
{
    return db->Execute(createTableQuery);
}

bool ContactsTable::Add(ContactsTableRow entry)
{
    return db->Execute("insert or ignore into contacts (name_id, numbers_id, ring_id, address_id, type, whitelist, "
                       "blacklist, favourites, speeddial ) VALUES "
                       "(%lu, '%q', %lu, %lu, %lu, %lu, %lu, %lu, '%q');",
                       entry.nameID,
                       entry.numbersID.c_str(),
                       entry.ringID,
                       entry.addressID,
                       entry.type,
                       entry.isOnWhitelist,
                       entry.isOnBlacklist,
                       entry.isOnFavourites,
                       entry.speedDial.c_str());
}

bool ContactsTable::RemoveByID(uint32_t id)
{
    return db->Execute("DELETE FROM contacts where _id = %u;", id);
}

bool ContactsTable::BlockByID(uint32_t id, bool shouldBeBlocked)
{
    return db->Execute("UPDATE contacts SET blacklist=%lu WHERE _id=%lu", shouldBeBlocked ? 1 : 0, id);
}

bool ContactsTable::Update(ContactsTableRow entry)
{
    return db->Execute("UPDATE contacts SET name_id = %lu, numbers_id = '%q', ring_id = %lu, address_id = %lu, type "
                       "= %lu, whitelist = %lu, blacklist = %lu, "
                       "favourites = %lu, speeddial = '%q' WHERE _id=%lu;",
                       entry.nameID,
                       entry.numbersID.c_str(),
                       entry.ringID,
                       entry.addressID,
                       entry.type,
                       entry.isOnWhitelist,
                       entry.isOnBlacklist,
                       entry.isOnFavourites,
                       entry.speedDial.c_str(),
                       entry.ID);
}

ContactsTableRow ContactsTable::GetByID(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM contacts WHERE _id= %lu;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return ContactsTableRow();
    }

    return ContactsTableRow{
        (*retQuery)[0].GetUInt32(),                           // ID
        (*retQuery)[1].GetUInt32(),                           // nameID
        (*retQuery)[2].GetString(),                           // numbersID
        (*retQuery)[3].GetUInt32(),                           // ringID
        (*retQuery)[4].GetUInt32(),                           // addressID
        static_cast<ContactType>((*retQuery)[5].GetUInt32()), // type
        (*retQuery)[6].GetBool(),                             // is on whitelist
        (*retQuery)[7].GetBool(),                             // is on blacklist
        (*retQuery)[8].GetBool(),                             // is on favourites
        (*retQuery)[9].GetString(),                           // speed dial key
    };
}

std::vector<ContactsTableRow> ContactsTable::Search(const std::string primaryName,
                                                    const std::string alternativeName,
                                                    const std::string number)
{
    std::vector<ContactsTableRow> ret;

    if (primaryName.empty() && alternativeName.empty() && number.empty()) {
        return (ret);
    }

    std::string q = "select t1.*,t2.name_primary,t2.name_alternative from contacts t1 inner join contact_name "
                    "t2 "
                    "on t1._id=t2.contact_id inner join contact_number t3 on t1._id=t3.contact_id where ";

    if (!primaryName.empty()) {
        q += "t2.name_primary like '%%" + primaryName + "%%'";
        if (!alternativeName.empty())
            q += " or ";
    }

    if (!alternativeName.empty()) {
        q += "t2.name_alternative like '%%" + alternativeName + "%%'";
        if (!number.empty())
            q += " or ";
    }

    if (!number.empty())
        q += "t3.number_e164 like '%%" + number + "%%'";

    LOG_DEBUG("query: \"%s\"", q.c_str());
    auto retQuery = db->Query(q.c_str());

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsTableRow>();
    }

    do {
        ret.push_back(ContactsTableRow{
            (*retQuery)[0].GetUInt32(),                           // ID
            (*retQuery)[1].GetUInt32(),                           // nameID
            (*retQuery)[2].GetString(),                           // numbersID
            (*retQuery)[3].GetUInt32(),                           // ringID
            (*retQuery)[4].GetUInt32(),                           // addressID
            static_cast<ContactType>((*retQuery)[5].GetUInt32()), // type
            (*retQuery)[6].GetBool(),                             // is on whitelist
            (*retQuery)[7].GetBool(),                             // is on blacklist
            (*retQuery)[8].GetBool(),                             // is on favourites
            (*retQuery)[9].GetString(),                           // speed dial key
            (*retQuery)[10].GetString(),                          // primaryName
            (*retQuery)[11].GetString(),                          // alternativeName (WTF!)
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<std::uint32_t> ContactsTable::GetIDsByTextNumber(const std::string &filter,
                                                             std::size_t limit,
                                                             std::size_t offset)
{
    std::vector<std::uint32_t> ids;
    std::string query = "SELECT DISTINCT contacts._id"
                        " FROM contacts"
                        " INNER JOIN contact_name ON contacts._id == contact_name.contact_id"
                        " INNER JOIN contact_number ON contacts._id == contact_number.contact_id"
                        " WHERE contact_number.number_user LIKE '%%" +
                        filter +
                        "%%'"
                        " ORDER BY contacts.favourites DESC, name_alternative || ' ' || name_primary";

    if (limit > 0) {
        query += " LIMIT " + std::to_string(limit);
        query += " OFFSET " + std::to_string(offset);
    }

    auto queryRet = db->Query(query.c_str());
    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return ids;
    }

    do {
        ids.push_back((*queryRet)[0].GetUInt32());
    } while (queryRet->NextRow());

    return ids;
}

std::vector<ContactsTableRow> ContactsTable::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->Query("SELECT * from contacts ORDER BY name_id LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsTableRow>();
    }

    std::vector<ContactsTableRow> ret;

    do {
        ret.push_back(ContactsTableRow{
            (*retQuery)[0].GetUInt32(),                           // ID
            (*retQuery)[1].GetUInt32(),                           // nameID
            (*retQuery)[2].GetString(),                           // numbersID
            (*retQuery)[3].GetUInt32(),                           // ringID
            (*retQuery)[4].GetUInt32(),                           // addressID
            static_cast<ContactType>((*retQuery)[5].GetUInt32()), // type
            (*retQuery)[6].GetBool(),                             // is on whitelist
            (*retQuery)[7].GetBool(),                             // is on blacklist
            (*retQuery)[8].GetBool(),                             // is on favourites
            (*retQuery)[9].GetString(),                           // speed dial key
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<ContactsTableRow> ContactsTable::GetLimitOffsetByField(uint32_t offset,
                                                                   uint32_t limit,
                                                                   ContactTableFields field,
                                                                   const char *str)
{

    std::string fieldName;
    switch (field) {
    case ContactTableFields ::SpeedDial:
        fieldName = "speeddial";
        break;
    case ContactTableFields::Favourite:
        fieldName = "favourites";
        break;
    default:
        return std::vector<ContactsTableRow>();
    }

    auto retQuery = db->Query("SELECT * from contacts WHERE %q='%q' ORDER BY name_id LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsTableRow>();
    }

    std::vector<ContactsTableRow> ret;

    do {
        ret.push_back(ContactsTableRow{
            (*retQuery)[0].GetUInt32(),                           // ID
            (*retQuery)[1].GetUInt32(),                           // nameID
            (*retQuery)[2].GetString(),                           // numbersID
            (*retQuery)[3].GetUInt32(),                           // ringID
            (*retQuery)[4].GetUInt32(),                           // addressID
            static_cast<ContactType>((*retQuery)[5].GetUInt32()), // type
            (*retQuery)[6].GetBool(),                             // is on whitelist
            (*retQuery)[7].GetBool(),                             // is on blacklist
            (*retQuery)[8].GetBool(),                             // is on favourites
            (*retQuery)[9].GetString(),                           // speed dial key
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t ContactsTable::GetCount()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contacts;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ContactsTable::GetCountByFieldID(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contacts WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}
