/*
 * @file NoteRecord.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "NotesRecord.hpp"
#include "../Tables/NotesTable.hpp"

NotesRecordInterface::NotesRecordInterface(NotesDB *notesDb) : notesDB(notesDb)
{}

NotesRecordInterface::~NotesRecordInterface()
{}

bool NotesRecordInterface::Add(const NotesRecord &rec)
{

    uint32_t contactID = 0;

    // Create SMS
    notesDB->notes.Add(NotesTableRow{.date = rec.date, .snippet = rec.snippet, .path = rec.path});

    // TODO: error check

    return true;
}

uint32_t NotesRecordInterface::GetCount()
{
    return notesDB->notes.GetCount();
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
    auto notes = notesDB->notes.GetLimitOffset(offset, limit);

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

    auto note = notesDB->notes.GetByID(rec.ID);
    if (note.ID == 0) {
        return false;
    }

    notesDB->notes.Update(NotesTableRow{.ID = rec.ID, .date = rec.date, .snippet = rec.snippet, .path = rec.path});

    return true;
}

bool NotesRecordInterface::RemoveByID(uint32_t id)
{

    auto note = notesDB->notes.GetByID(id);
    if (note.ID == 0) {
        return false;
    }

    // Remove SMS
    if (notesDB->notes.RemoveByID(id) == false) {
        return false;
    }

    return true;
}

bool NotesRecordInterface::RemoveByField(NotesRecordField field, const char *str)
{

    switch (field) {
    case NotesRecordField::Data:
        return notesDB->notes.RemoveByField(NotesTableFields::Date, str);
    default:
        return false;
    }
}

NotesRecord NotesRecordInterface::GetByID(uint32_t id)
{
    auto note = notesDB->notes.GetByID(id);

    return NotesRecord{.ID = note.ID, .date = note.date, .snippet = note.snippet, .path = note.path};
}
