/*
 * @file NotesTable.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Table.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct NotesTableRow{
    uint32_t ID;
    uint32_t date;
    UTF8    snippet; //up to 60 characters
    UTF8    path;
};

enum class NotesTableFields{
    Date,
    Snippet,
	Path
};

/*
 *
 */
class NotesTable : public Table<NotesTableRow,NotesTableFields> {
public:
	NotesTable(Database* db);
	virtual ~NotesTable();

	bool Create() override final;
	bool Add(NotesTableRow entry) override final;
	bool RemoveByID(uint32_t id) override final;
	bool RemoveByField(NotesTableFields field, const char* str) override final;
	bool Update(NotesTableRow entry) override final;
	NotesTableRow GetByID(uint32_t id) override final;
	std::vector<NotesTableRow> GetLimitOffset(uint32_t offset,uint32_t limit) override final;
	std::vector<NotesTableRow> GetLimitOffsetByField(uint32_t offset,uint32_t limit,NotesTableFields field,const char* str) override final;

	uint32_t GetCount() override final;
	uint32_t GetCountByFieldID(const char* field,uint32_t id) override final;

private:

    const char* createTableQuery =
            "CREATE TABLE IF NOT EXISTS notes("
            "_id INTEGER PRIMARY KEY,"
            "date INTEGER,"
            "snippet TEXT DEFAULT '',"
    		"path TEXT DEFAULT '')";
};

