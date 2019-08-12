/*
 * @file NoteRecord.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_DB_INTERFACE_NOTESRECORD_HPP_
#define MODULE_DB_INTERFACE_NOTESRECORD_HPP_

#include "Record.hpp"
#include <stdint.h>
#include "../Databases/NotesDB.hpp"
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"

struct NotesRecord{
	uint32_t ID;
	uint32_t date;
	UTF8    snippet;
	UTF8    path;
};

enum class NotesRecordField{
    Data,
    Snippet,
	Path
};

/*
 *
 */
class NotesRecordInterface : public RecordInterface<NotesRecord,NotesRecordField > {
public:

    NotesRecordInterface(NotesDB* notesDb);
    virtual ~NotesRecordInterface();

    bool Add(const NotesRecord& rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(NotesRecordField field,const char* str) override final;
    bool Update(const NotesRecord& rec) override final;
    NotesRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<NotesRecord>> GetLimitOffset(uint32_t offset,uint32_t limit) override final;

std::unique_ptr<std::vector<NotesRecord>> GetLimitOffsetByField(uint32_t offset,uint32_t limit,NotesRecordField field, const char* str) override final;

private:
    const uint32_t snippetLength = 60;
    NotesDB* notesDB;
};

#endif /* MODULE_DB_INTERFACE_NOTESRECORD_HPP_ */
