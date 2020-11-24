// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesGetForList.hpp"

namespace db::query
{
    NotesGetForList::NotesGetForList(unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), offset{offset}, limit{limit}
    {}

    std::string NotesGetForList::debugInfo() const
    {
        return {"NotesGetForList"};
    }

    NotesGetForListResult::NotesGetForListResult(std::vector<NotesRecord> notes) : records{std::move(notes)}
    {}

    const std::vector<NotesRecord> &NotesGetForListResult::getRecords() const
    {
        return records;
    }

    auto NotesGetForListResult::debugInfo() const -> std::string
    {
        return {"NotesGetForListResult"};
    }
} // namespace db::query
