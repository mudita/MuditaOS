// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <Common/Query.hpp>
#include "Interface/NotesRecord.hpp"

namespace db::query
{
    class QueryNotesGet : public Query
    {
      public:
        explicit QueryNotesGet(unsigned int offset = 0U, unsigned int limit = 0U);

        [[nodiscard]] unsigned int getOffset() const noexcept;
        [[nodiscard]] unsigned int getLimit() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        unsigned int offset;
        unsigned int limit;
    };

    class NotesGetResult : public QueryResult
    {
      public:
        NotesGetResult(std::vector<NotesRecord> notes, unsigned int dbRecordsCount);

        [[nodiscard]] const std::vector<NotesRecord> &getRecords() const noexcept;
        [[nodiscard]] unsigned int getCount() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        std::vector<NotesRecord> records;
        unsigned int dbRecordsCount;
    };
} // namespace db::query
