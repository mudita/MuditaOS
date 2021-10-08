// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNotesGet.hpp"

namespace db::query
{
    QueryNotesGet::QueryNotesGet(unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), offset{offset}, limit{limit}
    {}

    unsigned int QueryNotesGet::getOffset() const noexcept
    {
        return offset;
    }

    unsigned int QueryNotesGet::getLimit() const noexcept
    {
        return limit;
    }

    std::string QueryNotesGet::debugInfo() const
    {
        return {"QueryNotesGet"};
    }

    NotesGetResult::NotesGetResult(std::vector<NotesRecord> notes, unsigned int dbRecordsCount)
        : records{std::move(notes)}, dbRecordsCount{dbRecordsCount}
    {}

    const std::vector<NotesRecord> &NotesGetResult::getRecords() const noexcept
    {
        return records;
    }

    unsigned int NotesGetResult::getCount() const noexcept
    {
        return dbRecordsCount;
    }

    auto NotesGetResult::debugInfo() const -> std::string
    {
        return {"NotesGetResult"};
    }
} // namespace db::query
