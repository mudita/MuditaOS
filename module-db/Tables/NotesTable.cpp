// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesTable.hpp"
#include <log/log.hpp>
#include <string>

NotesTable::NotesTable(Database *db) : Table(db)
{}

bool NotesTable::create()
{
    return true;
}

bool NotesTable::add(NotesTableRow entry)
{
    return db->execute(
        "INSERT or ignore INTO notes ( date, snippet ) VALUES ( %lu, '%q' );", entry.date, entry.snippet.c_str());
}

bool NotesTable::removeAll()
{
    return db->execute("DELETE FROM notes;");
}

bool NotesTable::removeById(std::uint32_t id)
{
    return db->execute("DELETE FROM notes where _id = %lu;", id);
}

bool NotesTable::removeByField(NotesTableFields field, const char *str)
{
    std::string fieldName;
    switch (field) {
    case NotesTableFields::Date:
        fieldName = "date";
        break;
    case NotesTableFields::Snippet:
        fieldName = "snippet";
        break;
    default:
        return false;
    }
    return db->execute("DELETE FROM note where %q = '%q';", fieldName.c_str(), str);
}

bool NotesTable::update(NotesTableRow entry)
{
    return db->execute(
        "UPDATE notes SET date = %lu, snippet = '%q' WHERE _id = %lu;", entry.date, entry.snippet.c_str(), entry.ID);
}

NotesTableRow NotesTable::getById(std::uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM notes WHERE _id = %lu;", id);
    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return NotesTableRow();
    }
    return NotesTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getUInt32(), // date
        (*retQuery)[2].getString()  // snippet
    };
}

std::vector<NotesTableRow> NotesTable::getLimitOffset(std::uint32_t offset, std::uint32_t limit)
{
    auto retQuery = db->query("SELECT * FROM notes ORDER BY date DESC LIMIT %lu OFFSET %lu;", limit, offset);
    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<NotesTableRow>();
    }

    std::vector<NotesTableRow> ret;
    do {
        ret.push_back(NotesTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // date
            (*retQuery)[2].getString()  // snippet
        });
    } while (retQuery->nextRow());
    return ret;
}

std::vector<NotesTableRow> NotesTable::getLimitOffsetByField(std::uint32_t offset,
                                                             std::uint32_t limit,
                                                             NotesTableFields field,
                                                             const char *str)
{
    std::string fieldName;
    switch (field) {
    case NotesTableFields::Date:
        fieldName = "date";
        break;
    case NotesTableFields::Snippet:
        fieldName = "snippet";
        break;
    default:
        return std::vector<NotesTableRow>();
    }

    auto retQuery = db->query("SELECT * FROM notes WHERE %q='%q' ORDER BY date DESC LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);
    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<NotesTableRow>();
    }

    std::vector<NotesTableRow> ret;
    do {
        ret.push_back(NotesTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // date
            (*retQuery)[2].getString()  // snippet
        });
    } while (retQuery->nextRow());
    return ret;
}

std::pair<std::vector<NotesTableRow>, int> NotesTable::getByText(const std::string &text,
                                                                 unsigned int offset,
                                                                 unsigned int limit)
{

    int count          = 0;
    auto queryRet      = db->query("SELECT COUNT(*), INSTR(snippet,'%q') pos FROM notes WHERE pos > 0;", text.c_str());
    if (queryRet && queryRet->getRowCount() != 0) {
        count = (*queryRet)[0].getUInt32();
    }

    std::string queryText = "SELECT *, INSTR(snippet,'" + text + "') pos FROM notes WHERE pos > 0 LIMIT " +
                            std::to_string(limit) + " offset " + std::to_string(offset) + " ;";
    auto retQuery = db->query(queryText.c_str());

    if (retQuery == nullptr || retQuery->getRowCount() == 0) {
        return {{}, int(count)};
    }

    std::vector<NotesTableRow> records;
    do {
        NotesTableRow row{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // date
            (*retQuery)[2].getString()  // snippet
        };
        records.push_back(std::move(row));
    } while (retQuery->nextRow());

    return {records, count};
}

std::uint32_t NotesTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM notes;");
    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }
    return (*queryRet)[0].getUInt32();
}

std::uint32_t NotesTable::countByFieldId(const char *field, std::uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM notes WHERE %q = %lu;", field, id);
    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }
    return (*queryRet)[0].getUInt32();
}
