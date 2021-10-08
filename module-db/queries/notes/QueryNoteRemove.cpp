// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNoteRemove.hpp"

namespace db::query
{
    QueryNoteRemove::QueryNoteRemove(unsigned int recordId) : Query(Query::Type::Delete), recordId{recordId}
    {}

    unsigned int QueryNoteRemove::getRecordId() const noexcept
    {
        return recordId;
    }

    std::string QueryNoteRemove::debugInfo() const
    {
        return {"QueryNoteRemove"};
    }

    NoteRemoveResult::NoteRemoveResult(bool isSuccess) : isSuccess{isSuccess}
    {}

    bool NoteRemoveResult::succeed() const noexcept
    {
        return isSuccess;
    }

    auto NoteRemoveResult::debugInfo() const -> std::string
    {
        return {"NoteRemoveResult"};
    }
} // namespace db::query
