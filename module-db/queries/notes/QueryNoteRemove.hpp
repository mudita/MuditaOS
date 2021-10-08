// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <Common/Query.hpp>
#include "Interface/NotesRecord.hpp"

namespace db::query
{
    class QueryNoteRemove : public Query
    {
      public:
        explicit QueryNoteRemove(unsigned int recordId);
        [[nodiscard]] unsigned int getRecordId() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        unsigned int recordId;
    };

    class NoteRemoveResult : public QueryResult
    {
      public:
        explicit NoteRemoveResult(bool isSuccess);

        [[nodiscard]] bool succeed() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        bool isSuccess;
    };
} // namespace db::query
