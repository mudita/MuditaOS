// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <Common/Query.hpp>
#include "Interface/NotesRecord.hpp"

namespace db::query
{
    class QueryNoteStore : public Query
    {
      public:
        explicit QueryNoteStore(NotesRecord record);

        [[nodiscard]] const NotesRecord &getRecord() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        NotesRecord record;
    };

    class NoteStoreResult : public QueryResult
    {
      public:
        NoteStoreResult(bool isSuccess, std::uint32_t noteId);

        [[nodiscard]] bool succeed() const noexcept;
        [[nodiscard]] std::uint32_t getNoteId() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        bool isSuccess;
        std::uint32_t noteId;
    };
} // namespace db::query
