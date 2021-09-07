// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesRecord.hpp"

#include "queries/notes/QueryNotesGet.hpp"
#include "queries/notes/QueryNotesGetByText.hpp"
#include "queries/notes/QueryNoteStore.hpp"
#include "queries/notes/QueryNoteRemove.hpp"

#include <cassert>

namespace
{
    NotesTableFields toNotesTableFields(NotesRecordField field)
    {
        switch (field) {
        case NotesRecordField::Date:
            return NotesTableFields::Date;
        case NotesRecordField::Snippet:
            return NotesTableFields::Snippet;
        }
        throw std::invalid_argument{"Invalid notes record field passed."};
    }
} // namespace

NotesRecordInterface::NotesRecordInterface(NotesDB *notesDb) : notesDB(notesDb)
{}

bool NotesRecordInterface::Add(const NotesRecord &rec)
{
    return notesDB->notes.add(NotesTableRow{.date = rec.date, .snippet = rec.snippet});
}

std::uint32_t NotesRecordInterface::GetCount()
{
    return notesDB->notes.count();
}

std::unique_ptr<std::vector<NotesRecord>> NotesRecordInterface::GetLimitOffsetByField(std::uint32_t offset,
                                                                                      std::uint32_t limit,
                                                                                      NotesRecordField field,
                                                                                      const char *str)
{
    auto records     = std::make_unique<std::vector<NotesRecord>>();
    const auto notes = notesDB->notes.getLimitOffsetByField(offset, limit, toNotesTableFields(field), str);
    for (const auto &w : notes) {
        NotesRecord record{w.ID, w.date, w.snippet};
        records->push_back(std::move(record));
    }
    return records;
}

std::unique_ptr<std::vector<NotesRecord>> NotesRecordInterface::GetLimitOffset(std::uint32_t offset,
                                                                               std::uint32_t limit)
{
    return std::make_unique<std::vector<NotesRecord>>(getNotes(offset, limit));
}

bool NotesRecordInterface::Update(const NotesRecord &rec)
{
    auto note = notesDB->notes.getById(rec.ID);
    if (note.ID == DB_ID_NONE) {
        return false;
    }

    notesDB->notes.update(NotesTableRow{.ID = rec.ID, .date = rec.date, .snippet = rec.snippet});
    return true;
}

bool NotesRecordInterface::RemoveAll()
{
    return notesDB->notes.removeAll();
}

bool NotesRecordInterface::RemoveByID(std::uint32_t id)
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

NotesRecord NotesRecordInterface::GetByID(std::uint32_t id)
{
    auto note = notesDB->notes.getById(id);
    return NotesRecord{note.ID, note.date, note.snippet};
}

std::vector<NotesRecord> NotesRecordInterface::getNotes(std::uint32_t offset, std::uint32_t limit) const
{
    std::vector<NotesRecord> records;
    const auto &notes = notesDB->notes.getLimitOffset(offset, limit);
    for (const auto &note : notes) {
        NotesRecord record{note.ID, note.date, note.snippet};
        records.push_back(std::move(record));
    }
    return records;
}

std::pair<std::vector<NotesRecord>, unsigned int> NotesRecordInterface::getNotesByText(const std::string &text,
                                                                                       unsigned int offset,
                                                                                       unsigned int limit) const
{
    std::vector<NotesRecord> records;
    auto [notes, count] = notesDB->notes.getByText(text, offset, limit);
    for (const auto &note : notes) {
        NotesRecord record{note.ID, note.date, note.snippet};
        records.push_back(std::move(record));
    }
    return {records, count};
}

std::unique_ptr<db::QueryResult> NotesRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (typeid(*query) == typeid(db::query::QueryNotesGet)) {
        return getQuery(query);
    }
    if (typeid(*query) == typeid(db::query::QueryNotesGetByText)) {
        return getByTextQuery(query);
    }
    if (typeid(*query) == typeid(db::query::QueryNoteStore)) {
        return storeQuery(query);
    }
    if (typeid(*query) == typeid(db::query::QueryNoteRemove)) {
        return removeQuery(query);
    }
    return nullptr;
}

std::unique_ptr<db::QueryResult> NotesRecordInterface::getQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<db::query::QueryNotesGet *>(query.get());
    const auto &records   = getNotes(localQuery->getOffset(), localQuery->getLimit());
    auto response         = std::make_unique<db::query::NotesGetResult>(records, GetCount());
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> NotesRecordInterface::getByTextQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<db::query::QueryNotesGetByText *>(query.get());
    auto [records, count] = getNotesByText(localQuery->getText(), localQuery->getOffset(), localQuery->getLimit());
    auto response         = std::make_unique<db::query::NotesGetByTextResult>(records, count);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> NotesRecordInterface::storeQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery  = static_cast<db::query::QueryNoteStore *>(query.get());
    const auto &record     = localQuery->getRecord();
    bool isSuccess         = false;
    std::uint32_t resultId = DB_ID_NONE;

    if (const auto id = notesDB->notes.getById(record.ID).ID; id != DB_ID_NONE) {
        // Already exists in the DB.
        isSuccess = Update(record);
        resultId  = id;
    }
    else {
        isSuccess = Add(record);
        if (isSuccess) {
            resultId = notesDB->getLastInsertRowId();
        }
    }

    auto response = std::make_unique<db::query::NoteStoreResult>(isSuccess, resultId);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> NotesRecordInterface::removeQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<db::query::QueryNoteRemove *>(query.get());
    const auto isSuccess  = RemoveByID(localQuery->getRecordId());
    auto response         = std::make_unique<db::query::NoteRemoveResult>(isSuccess);
    response->setRequestQuery(query);
    return response;
}
