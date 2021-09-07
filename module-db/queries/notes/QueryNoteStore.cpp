// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNoteStore.hpp"

namespace db::query
{
    QueryNoteStore::QueryNoteStore(NotesRecord record) : Query(Query::Type::Create), record{std::move(record)}
    {}

    const NotesRecord &QueryNoteStore::getRecord() const noexcept
    {
        return record;
    }

    std::string QueryNoteStore::debugInfo() const
    {
        return {"QueryNoteStore"};
    }

    NoteStoreResult::NoteStoreResult(bool isSuccess, std::uint32_t noteId) : isSuccess{isSuccess}, noteId{noteId}
    {}

    bool NoteStoreResult::succeed() const noexcept
    {
        return isSuccess;
    }

    std::uint32_t NoteStoreResult::getNoteId() const noexcept
    {
        return noteId;
    }

    auto NoteStoreResult::debugInfo() const -> std::string
    {
        return {"NoteStoreResult"};
    }
} // namespace db::query
