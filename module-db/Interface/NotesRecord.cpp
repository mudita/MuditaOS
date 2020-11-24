// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesRecord.hpp"

#include "queries/notes/NotesGetForList.hpp"

#include <cassert>

namespace
{
    NotesTableFields toNotesTableFields(NotesRecordField field) noexcept
    {
        switch (field) {
        case NotesRecordField::Date:
            return NotesTableFields::Date;
        case NotesRecordField::Snippet:
            return NotesTableFields::Snippet;
        case NotesRecordField::Path:
            return NotesTableFields::Path;
        }
        assert(false);
    }
} // namespace

NotesRecordInterface::NotesRecordInterface(NotesDB *notesDb) : notesDB(notesDb)
{}

bool NotesRecordInterface::Add(const NotesRecord &rec)
{
    return notesDB->notes.add(NotesTableRow{.date = rec.date, .snippet = rec.snippet, .path = rec.path.string()});
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
    auto records     = std::make_unique<std::vector<NotesRecord>>();
    const auto notes = notesDB->notes.getLimitOffsetByField(offset, limit, toNotesTableFields(field), str);
    for (const auto &w : notes) {
        NotesRecord record{w.ID, w.date, w.snippet, std::filesystem::path{w.path}};
        records->push_back(std::move(record));
    }
    return records;
}

std::unique_ptr<std::vector<NotesRecord>> NotesRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    return std::make_unique<std::vector<NotesRecord>>(getNotes(offset, limit));
}

bool NotesRecordInterface::Update(const NotesRecord &rec)
{
    auto note = notesDB->notes.getById(rec.ID);
    if (note.ID == DB_ID_NONE) {
        return false;
    }

    notesDB->notes.update(
        NotesTableRow{.ID = rec.ID, .date = rec.date, .snippet = rec.snippet, .path = rec.path.string()});
    return true;
}

bool NotesRecordInterface::RemoveByID(uint32_t id)
{
    return notesDB->notes.removeById(id);
}

bool NotesRecordInterface::RemoveByField(NotesRecordField field, const char *str)
{
    switch (field) {
    case NotesRecordField::Date:
        return notesDB->notes.removeByField(NotesTableFields::Date, str);
    default:
        return false;
    }
}

NotesRecord NotesRecordInterface::GetByID(uint32_t id)
{
    auto note = notesDB->notes.getById(id);
    return NotesRecord{note.ID, note.date, note.snippet, std::filesystem::path{note.path}};
}

std::vector<NotesRecord> NotesRecordInterface::getNotes(uint32_t offset, uint32_t limit) const
{
    std::vector<NotesRecord> records;
    const auto notes = notesDB->notes.getLimitOffset(offset, limit);
    for (const auto &w : notes) {
        NotesRecord record{w.ID, w.date, w.snippet, std::filesystem::path{w.path}};
        records.push_back(std::move(record));
    }
    return records;
}

std::unique_ptr<db::QueryResult> NotesRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    LOG_ERROR("NOTES COUNT: %u", GetCount());
    if (typeid(*query) == typeid(db::query::NotesGetForList)) {
        return getForListQuery(query);
    }
    return nullptr;
}

std::unique_ptr<db::QueryResult> NotesRecordInterface::getForListQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<db::query::NotesGetForList *>(query.get());
    const auto &records   = getNotes(localQuery->offset, localQuery->limit);
    auto response         = std::make_unique<db::query::NotesGetForListResult>(records);
    response->setRequestQuery(query);
    return response;
}
