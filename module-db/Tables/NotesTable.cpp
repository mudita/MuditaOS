/*
 * @file NotesTable.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "NotesTable.hpp"


NotesTable::NotesTable(Database *db) : Table(db) {
}

NotesTable::~NotesTable() {

}

bool NotesTable::Create() {
    return db->Execute(createTableQuery);
}

bool NotesTable::Add(NotesTableRow entry) {
    return db->Execute(
            "INSERT or ignore INTO notes ( date, snippet, path ) VALUES (%lu,'%s','%s');",
            entry.date,
            entry.snippet.c_str(),
			entry.path.c_str()
    );
}

bool NotesTable::RemoveByID(uint32_t id) {
    return db->Execute("DELETE FROM notes where _id = %lu;", id);
}

bool NotesTable::RemoveByField(NotesTableFields field, const char *str) {
    std::string fieldName;

    switch(field){
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

    return db->Execute("DELETE FROM note where %s = '%s';", fieldName.c_str(),str);

}


bool NotesTable::Update(NotesTableRow entry) {
    return db->Execute(
            "UPDATE notes SET date = %lu, snippet = '%s', , snippet = '%s' WHERE _id=%lu;",
            entry.date,
            entry.snippet.c_str(),
			entry.path.c_str(),
            entry.ID
    );
}

NotesTableRow NotesTable::GetByID(uint32_t id) {
    auto retQuery = db->Query("SELECT * FROM notes WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return NotesTableRow();
    }

    return NotesTableRow{(*retQuery)[0].GetUInt32(),  // ID
                       (*retQuery)[1].GetUInt32(),    // date
                       (*retQuery)[2].GetString(),    //snippet
					   (*retQuery)[3].GetString(),    //path
    };
}

std::vector<NotesTableRow> NotesTable::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto retQuery = db->Query("SELECT * from notes ORDER BY date LIMIT %lu OFFSET %lu;",
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<NotesTableRow>();
    }

    std::vector<NotesTableRow> ret;

    do {
    	ret.push_back(
    		NotesTableRow{(*retQuery)[0].GetUInt32(),  // ID
    			(*retQuery)[1].GetUInt32(),    // date
				(*retQuery)[2].GetString(),    //snippet
				(*retQuery)[3].GetString(),    //path
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<NotesTableRow>
NotesTable::GetLimitOffsetByField(uint32_t offset, uint32_t limit, NotesTableFields field, const char *str) {

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

    auto retQuery = db->Query("SELECT * from notes WHERE %s='%s' ORDER BY date LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<NotesTableRow>();
    }

    std::vector<NotesTableRow> ret;

    do {
        ret.push_back(NotesTableRow{(*retQuery)[0].GetUInt32(),  // ID
            (*retQuery)[1].GetUInt32(),    // date
            (*retQuery)[2].GetString(),    //snippet
			(*retQuery)[3].GetString(),    //path
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t NotesTable::GetCount() {
    auto queryRet = db->Query("SELECT COUNT(*) FROM NOTES;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t NotesTable::GetCountByFieldID(const char *field, uint32_t id) {
    auto queryRet = db->Query("SELECT COUNT(*) FROM notes WHERE %s=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

