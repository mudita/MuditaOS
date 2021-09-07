// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNotesGetByText.hpp"

#include <utility>

namespace db::query
{
    QueryNotesGetByText::QueryNotesGetByText(std::string text, unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), offset(offset), limit(limit), text(std::move(text))
    {}

    const std::string &QueryNotesGetByText::getText() const noexcept
    {
        return text;
    }

    std::string QueryNotesGetByText::debugInfo() const
    {
        return {"QueryNotesGetByText"};
    }

    NotesGetByTextResult::NotesGetByTextResult(std::vector<NotesRecord> notes, unsigned int count)
        : records{std::move(notes)}, dbRecordsCount(count)
    {}

    const std::vector<NotesRecord> &NotesGetByTextResult::getRecords() const noexcept
    {
        return records;
    }

    auto NotesGetByTextResult::debugInfo() const -> std::string
    {
        return {"NotesGetByTextResult"};
    }
} // namespace db::query
