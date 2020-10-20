// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesRecord.hpp"
#include "../Tables/NotesTable.hpp"

NotesRecordInterface::NotesRecordInterface(NotesDB *notesDb) : notesDB(notesDb)
{}

NotesRecordInterface::~NotesRecordInterface()
{}

bool NotesRecordInterface::Add(const NotesRecord &rec)
{
    // Create SMS
    notesDB->notes.add(NotesTableRow{.date = rec.date, .snippet = rec.snippet, .path = rec.path});

    // TODO: error check

    return true;
}

uint32_t NotesRecordInterface::GetCount()
{
    return notesDB->notes.count();
}

std::unique_ptr<std::vector<NotesRecord>> NotesRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                      uint32_t limit,
                                                                                      NotesRecordField field,
                                                                                      const char *str)
{
    return GetLimitOffset(offset, limit);
}

std::unique_ptr<std::vector<NotesRecord>> NotesRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto notes = notesDB->notes.getLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<NotesRecord>>();
    //
    NotesRecordInterface notesInterface(notesDB);
    for (const auto &w : notes) {

        records->push_back({
            .ID      = w.ID,
            .date    = w.date,
            .snippet = w.snippet,
            .path    = w.path,
        });
    }

    return records;
}

bool NotesRecordInterface::Update(const NotesRecord &rec)
{

    auto note = notesDB->notes.getById(rec.ID);
    if (note.ID == 0) {
        return false;
    }

    notesDB->notes.update(NotesTableRow{.ID = rec.ID, .date = rec.date, .snippet = rec.snippet, .path = rec.path});

    return true;
}

bool NotesRecordInterface::RemoveByID(uint32_t id)
{

    auto note = notesDB->notes.getById(id);
    if (note.ID == 0) {
        return false;
    }

    // Remove SMS
    if (notesDB->notes.removeById(id) == false) {
        return false;
    }

    return true;
}

bool NotesRecordInterface::RemoveByField(NotesRecordField field, const char *str)
{

    switch (field) {
    case NotesRecordField::Data:
        return notesDB->notes.removeByField(NotesTableFields::Date, str);
    default:
        return false;
    }
}

NotesRecord NotesRecordInterface::GetByID(uint32_t id)
{
    auto note = notesDB->notes.getById(id);

    return NotesRecord{.ID = note.ID, .date = note.date, .snippet = note.snippet, .path = note.path};
}
