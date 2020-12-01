// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNotesGetByText.hpp"

namespace db::query
{
    QueryNotesGetByText::QueryNotesGetByText(std::string text) : Query(Query::Type::Read), text{std::move(text)}
    {}

    const std::string &QueryNotesGetByText::getText() const noexcept
    {
        return text;
    }

    std::string QueryNotesGetByText::debugInfo() const
    {
        return {"QueryNotesGetByText"};
    }

    NotesGetByTextResult::NotesGetByTextResult(std::vector<NotesRecord> notes) : records{std::move(notes)}
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
