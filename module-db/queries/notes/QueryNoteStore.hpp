// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
        explicit NoteStoreResult(bool isSuccess);

        [[nodiscard]] bool succeed() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        bool isSuccess;
    };
} // namespace db::query
