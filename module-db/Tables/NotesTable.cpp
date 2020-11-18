// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesTable.hpp"

NotesTable::NotesTable(Database *db) : Table(db)
{}

NotesTable::~NotesTable()
{}

bool NotesTable::create()
{
    return true;
}

bool NotesTable::add(NotesTableRow entry)
{
    return db->execute("INSERT or ignore INTO notes ( date, snippet, path ) VALUES (%lu,'%q','%q');",
                       entry.date,
                       entry.snippet.c_str(),
                       entry.path.c_str());
}

bool NotesTable::removeById(uint32_t id)
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

    case NotesTableFields::Path:
        fieldName = "path";
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
    return db->execute("UPDATE notes SET date = %lu, snippet = '%q', , snippet = '%q' WHERE _id=%lu;",
                       entry.date,
                       entry.snippet.c_str(),
                       entry.path.c_str(),
                       entry.ID);
}

NotesTableRow NotesTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM notes WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return NotesTableRow();
    }

    return NotesTableRow{
        (*retQuery)[0].getUInt32(), // ID
        (*retQuery)[1].getUInt32(), // date
        (*retQuery)[2].getString(), // snippet
        (*retQuery)[3].getString(), // path
    };
}

std::vector<NotesTableRow> NotesTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from notes ORDER BY date LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<NotesTableRow>();
    }

    std::vector<NotesTableRow> ret;

    do {
        ret.push_back(NotesTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // date
            (*retQuery)[2].getString(), // snippet
            (*retQuery)[3].getString(), // path
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<NotesTableRow> NotesTable::getLimitOffsetByField(uint32_t offset,
                                                             uint32_t limit,
                                                             NotesTableFields field,
                                                             const char *str)
{

    std::string fieldName;
    switch (field) {
    case NotesTableFields::Date:
        fieldName = "date";
        break;
    case NotesTableFields ::Snippet:
        fieldName = "snippet";
        break;
    case NotesTableFields ::Path:
        fieldName = "path";
        break;
    default:
        return std::vector<NotesTableRow>();
    }

    auto retQuery = db->query(
        "SELECT * from notes WHERE %q='%q' ORDER BY date LIMIT %lu OFFSET %lu;", fieldName.c_str(), str, limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<NotesTableRow>();
    }

    std::vector<NotesTableRow> ret;

    do {
        ret.push_back(NotesTableRow{
            (*retQuery)[0].getUInt32(), // ID
            (*retQuery)[1].getUInt32(), // date
            (*retQuery)[2].getString(), // snippet
            (*retQuery)[3].getString(), // path
        });
    } while (retQuery->nextRow());

    return ret;
}

uint32_t NotesTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM NOTES;");

    if (queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t NotesTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM notes WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}
