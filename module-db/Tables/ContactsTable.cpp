// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactsTable.hpp"
#include <log/log.hpp>
#include <Utils.hpp>

namespace ColumnName
{
    const uint8_t id         = 0;
    const uint8_t name_id    = 1;
    const uint8_t numbers_id = 2;
    const uint8_t ring_id    = 3;
    const uint8_t address_id = 4;
    const uint8_t speeddial  = 5;
}; // namespace ColumnName

namespace statements
{
    const auto selectWithoutTemp = "SELECT * FROM contacts WHERE _id= %lu"
                                   " AND "
                                   " contacts._id NOT IN ( "
                                   "   SELECT cmg.contact_id "
                                   "   FROM contact_match_groups cmg, contact_groups cg "
                                   "   WHERE cmg.group_id = cg._id "
                                   "       AND cg.name = 'Temporary' "
                                   "   ) ";
    const auto selectWithTemp = "SELECT * FROM contacts WHERE _id= %lu";
} // namespace statements

ContactsTable::ContactsTable(Database *db) : Table(db)
{}

ContactsTable::~ContactsTable()
{}

bool ContactsTable::create()
{
    return true;
}

bool ContactsTable::add(ContactsTableRow entry)
{
    return db->execute("insert or ignore into contacts (name_id, numbers_id, ring_id, address_id, speeddial) "
                       " VALUES (%lu, '%q', %lu, %lu, '%q');",
                       entry.nameID,
                       entry.numbersID.c_str(),
                       entry.ringID,
                       entry.addressID,
                       entry.speedDial.c_str());
}

bool ContactsTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM contacts where _id = %u;", id);
}

bool ContactsTable::BlockByID(uint32_t id, bool shouldBeBlocked)
{
    return db->execute("UPDATE contacts SET blacklist=%lu WHERE _id=%lu", shouldBeBlocked ? 1 : 0, id);
}

bool ContactsTable::update(ContactsTableRow entry)
{
    return db->execute("UPDATE contacts SET name_id = %lu, numbers_id = '%q', ring_id = %lu, address_id = %lu, "
                       " speeddial = '%q' WHERE _id=%lu;",
                       entry.nameID,
                       entry.numbersID.c_str(),
                       entry.ringID,
                       entry.addressID,
                       entry.speedDial.c_str(),
                       entry.ID);
}

ContactsTableRow ContactsTable::getById(uint32_t id)
{
    auto retQuery = db->query(statements::selectWithoutTemp, id);
    return getByIdCommon(std::move(retQuery));
}

ContactsTableRow ContactsTable::getByIdWithTemporary(uint32_t id)
{
    debug_db_data("%s", __FUNCTION__);
    auto retQuery = db->query(statements::selectWithTemp, id);
    return getByIdCommon(std::move(retQuery));
}

ContactsTableRow ContactsTable::getByIdCommon(std::unique_ptr<QueryResult> retQuery)
{
    debug_db_data("%s", __FUNCTION__);
    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        LOG_DEBUG("no results");
        return ContactsTableRow();
    }

    debug_db_data(
        "got results: %" PRIu32 "; ID: %" PRIu32, retQuery->getRowCount(), (*retQuery)[ColumnName::id].getInt32());
    return ContactsTableRow{
        Record((*retQuery)[ColumnName::id].getUInt32()),
        .nameID    = (*retQuery)[ColumnName::name_id].getUInt32(),
        .numbersID = (*retQuery)[ColumnName::numbers_id].getString(),
        .ringID    = (*retQuery)[ColumnName::ring_id].getUInt32(),
        .addressID = (*retQuery)[ColumnName::address_id].getUInt32(),
        .speedDial = (*retQuery)[ColumnName::speeddial].getString(),
    };
}

std::vector<ContactsTableRow> ContactsTable::Search(const std::string &primaryName,
                                                    const std::string &alternativeName,
                                                    const std::string &number)
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

    debug_db_data("query: \"%s\"", q.c_str());
    auto retQuery = db->query(q.c_str());

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsTableRow>();
    }
    if (retQuery->getRowCount() > 0)
        do {
            ret.push_back(ContactsTableRow{
                {(*retQuery)[ColumnName::id].getUInt32()},
                (*retQuery)[ColumnName::name_id].getUInt32(),
                (*retQuery)[ColumnName::numbers_id].getString(),
                (*retQuery)[ColumnName::ring_id].getUInt32(),
                (*retQuery)[ColumnName::address_id].getUInt32(),
                (*retQuery)[ColumnName::speeddial].getString(),
                (*retQuery)[ColumnName::speeddial + 1].getString(), // primaryName
                (*retQuery)[ColumnName::speeddial + 2].getString(), // alternativeName (WTF!)
            });
        } while (retQuery->nextRow());

    return ret;
}

std::string ContactsTable::GetSortedByNameQueryString(ContactQuerySection section)
{
    std::string query;
    if (section == ContactQuerySection::Favourites) {
        query = "SELECT contacts._id  FROM contacts"
                " INNER JOIN contact_name ON contact_name.contact_id == contacts._id "
                " LEFT JOIN contact_match_groups ON contact_match_groups.contact_id == contacts._id AND "
                " contact_match_groups.group_id = 1 "
                " WHERE group_id= 1 "
                " ORDER BY  (contact_name.name_alternative ='') ASC "
                " , UPPER(contact_name.name_alternative) ; ";
    }
    else if (section == ContactQuerySection::Mixed) {
        query = " SELECT contacts._id, contact_name.name_alternative  FROM contacts "
                " INNER JOIN contact_name ON contact_name.contact_id == contacts._id "
                " LEFT JOIN contact_match_groups ON contact_match_groups.contact_id == contacts._id AND "
                " contact_match_groups.group_id = 1 "
                " WHERE contacts._id not in ( "
                "    SELECT cmg.contact_id "
                "    FROM contact_match_groups cmg, contact_groups cg "
                "    WHERE cmg.group_id = cg._id "
                "       AND cg.name = 'Temporary' ) "
                " ORDER BY  (contact_name.name_alternative ='') ASC "
                " , UPPER(contact_name.name_alternative) ; ";
    }
    return query;
}

std::vector<std::uint32_t> ContactsTable::GetIDsSortedByName(std::uint32_t limit, std::uint32_t offset)
{
    std::vector<std::uint32_t> ids;
    std::vector<std::uint32_t> ids_limit;

    std::string query = GetSortedByNameQueryString(ContactQuerySection::Favourites);
    debug_db_data("query: %s", query.c_str());
    auto queryRet = db->query(query.c_str());
    if (queryRet == nullptr) {

        return ids;
    }
    if (queryRet->getRowCount() > 0)
        do {
            ids.push_back((*queryRet)[0].getUInt32());
        } while (queryRet->nextRow());

    query = GetSortedByNameQueryString(ContactQuerySection::Mixed);
    debug_db_data("query: %s", query.c_str());
    queryRet = db->query(query.c_str());
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return ids;
    }
    if (queryRet->getRowCount() > 0)
        do {
            ids.push_back((*queryRet)[0].getUInt32());
        } while (queryRet->nextRow());

    if (limit > 0) {
        for (uint32_t a = 0; a < limit; a++) {
            ids_limit.push_back(ids[a + offset]);
        }
        return ids_limit;
    }

    return ids;
}

ContactsMapData ContactsTable::GetPosOfFirstLetters()
{
    ContactsMapData contactMap;
    std::string FirstLetterOfName;
    std::string FirstLetterOfNameOld;
    std::uint32_t PositionOnList  = 0;
    std::uint32_t favouritesCount = 0;
    std::string query;

    query         = GetSortedByNameQueryString(ContactQuerySection::Favourites);
    auto queryRet = db->query(query.c_str());
    if (queryRet == nullptr) {
        return contactMap;
    }
    if (queryRet->getRowCount() > 0)
        do {
            favouritesCount++;
            PositionOnList++;
        } while (queryRet->nextRow());

    query    = GetSortedByNameQueryString(ContactQuerySection::Mixed);
    queryRet = db->query(query.c_str());
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return contactMap;
    }
    if (queryRet->getRowCount() > 0)
        do {
            UTF8 FirstLetterOfNameUtf = (*queryRet)[1].getString();
            FirstLetterOfName         = FirstLetterOfNameUtf.substr(0, 1);
            if (FirstLetterOfName != FirstLetterOfNameOld) {
                contactMap.firstLetterDictionary.insert(
                    std::pair<std::string, std::uint32_t>(FirstLetterOfName, PositionOnList));
            }
            FirstLetterOfNameOld = FirstLetterOfName;
            PositionOnList++;
        } while (queryRet->nextRow());

    contactMap.favouritesCount = favouritesCount;
    contactMap.itemCount       = PositionOnList;

    return contactMap;
}

std::vector<std::uint32_t> ContactsTable::GetIDsSortedByField(
    MatchType matchType, const std::string &name, std::uint32_t groupId, std::uint32_t limit, std::uint32_t offset)
{
    std::vector<std::uint32_t> ids;

    std::string query = "SELECT DISTINCT contacts._id FROM contacts";

    query += " INNER JOIN contact_name ON contact_name.contact_id == contacts._id ";
    query += " LEFT JOIN contact_match_groups ON contact_match_groups.contact_id == contacts._id AND "
             "contact_match_groups.group_id = " +
             std::to_string(groupId);

    constexpr auto exclude_temporary = " WHERE contacts._id not in ( "
                                       "   SELECT cmg.contact_id "
                                       "   FROM contact_match_groups cmg, contact_groups cg "
                                       "   WHERE cmg.group_id = cg._id "
                                       "       AND cg.name = 'Temporary' "
                                       "   ) ";

    switch (matchType) {
    case MatchType::Name: {
        query += exclude_temporary;

        if (!name.empty()) {
            const auto names     = utils::split(name, " ");
            const auto namePart1 = names[0];
            const auto namePart2 = names.size() > 1 ? names[1] : "";

            if (!namePart1.empty() && !namePart2.empty()) {
                query += " AND (( contact_name.name_primary LIKE '" + namePart1 + "%%'";
                query += " AND contact_name.name_alternative  LIKE '" + namePart2 + "%%')";
                query += " OR ( contact_name.name_primary LIKE '" + namePart2 + "%%'";
                query += " AND contact_name.name_alternative  LIKE '" + namePart1 + "%%'))";
            }
            else {
                query += " AND ( contact_name.name_primary LIKE '" + namePart1 + "%%'";
                query += " OR contact_name.name_alternative  LIKE '" + namePart1 + "%%')";
            }
        }
    } break;

    case MatchType::TextNumber: {
        if (!name.empty()) {
            query += " INNER JOIN contact_number ON contact_number.contact_id == contacts._id AND "
                     "contact_number.number_user LIKE '%%" +
                     name + "%%'";
        }
        query += exclude_temporary;
    } break;

    case MatchType::Group:
        query += " WHERE contact_match_groups.group_id == " + std::to_string(groupId);
        break;

    case MatchType::None: {
        query += exclude_temporary;
    } break;
    }

    query += " ORDER BY group_id DESC ";
    query += " , (contact_name.name_alternative IS NULL OR contact_name.name_alternative ='') ";
    query += " AND (contact_name.name_primary IS NULL OR contact_name.name_primary ='') ASC ";
    query += " , UPPER(contact_name.name_alternative || contact_name.name_primary) ";

    if (limit > 0) {
        query += " LIMIT " + std::to_string(limit);
        query += " OFFSET " + std::to_string(offset);
    }

    query += " ;";

    debug_db_data("query: %s", query.c_str());
    auto queryRet = db->query(query.c_str());
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return ids;
    }
    if (queryRet->getRowCount() > 0)
        do {
            ids.push_back((*queryRet)[0].getUInt32());
        } while (queryRet->nextRow());

    return ids;
}

std::vector<ContactsTableRow> ContactsTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from contacts WHERE contacts._id NOT IN "
                              " ( SELECT cmg.contact_id "
                              "    FROM contact_match_groups cmg, contact_groups cg "
                              "    WHERE cmg.group_id = cg._id "
                              "        AND cg.name = 'Temporary' "
                              " ) "
                              "ORDER BY name_id LIMIT %lu OFFSET %lu;",
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsTableRow>();
    }

    std::vector<ContactsTableRow> ret;
    if (retQuery->getRowCount() > 0)
        do {
            ret.push_back(ContactsTableRow{
                {(*retQuery)[ColumnName::id].getUInt32()},       // ID
                (*retQuery)[ColumnName::name_id].getUInt32(),    // nameID
                (*retQuery)[ColumnName::numbers_id].getString(), // numbersID
                (*retQuery)[ColumnName::ring_id].getUInt32(),    // ringID
                (*retQuery)[ColumnName::address_id].getUInt32(), // addressID
                (*retQuery)[ColumnName::speeddial].getString(),  // speed dial key
            });
        } while (retQuery->nextRow());

    return ret;
}

std::vector<ContactsTableRow> ContactsTable::getLimitOffsetByField(uint32_t offset,
                                                                   uint32_t limit,
                                                                   ContactTableFields field,
                                                                   const char *str)
{

    std::string fieldName;
    switch (field) {
    case ContactTableFields ::SpeedDial:
        fieldName = "speeddial";
        break;
    default:
        return std::vector<ContactsTableRow>();
    }

    auto retQuery = db->query("SELECT * from contacts WHERE %q='%q' ORDER BY name_id LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsTableRow>();
    }

    std::vector<ContactsTableRow> ret;
    if (retQuery->getRowCount() > 0)
        do {
            ret.push_back(ContactsTableRow{
                {(*retQuery)[ColumnName::id].getUInt32()},
                (*retQuery)[ColumnName::name_id].getUInt32(),
                (*retQuery)[ColumnName::numbers_id].getString(),
                (*retQuery)[ColumnName::ring_id].getUInt32(),
                (*retQuery)[ColumnName::address_id].getUInt32(),
                (*retQuery)[ColumnName::speeddial].getString(),
            });
        } while (retQuery->nextRow());

    return ret;
}

uint32_t ContactsTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contacts "
                              " WHERE contacts._id not in ( "
                              "    SELECT cmg.contact_id "
                              "    FROM contact_match_groups cmg, contact_groups cg "
                              "    WHERE cmg.group_id = cg._id "
                              "        AND cg.name = 'Temporary' "
                              "    ); ");

    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t ContactsTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contacts WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}
