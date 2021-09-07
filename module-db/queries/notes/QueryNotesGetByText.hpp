// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <Common/Query.hpp>
#include "Interface/NotesRecord.hpp"

namespace db::query
{
    class QueryNotesGetByText : public Query
    {
      public:
        explicit QueryNotesGetByText(std::string text, unsigned int offset, unsigned int limit);

        [[nodiscard]] const std::string &getText() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

        [[nodiscard]] unsigned int getOffset() const noexcept
        {
            return offset;
        }
        [[nodiscard]] unsigned int getLimit() const noexcept
        {
            return limit;
        };

      private:
        unsigned int offset;
        unsigned int limit;
        std::string text;
    };

    class NotesGetByTextResult : public QueryResult
    {
      public:
        explicit NotesGetByTextResult(std::vector<NotesRecord> notes, unsigned int count);

        [[nodiscard]] const std::vector<NotesRecord> &getRecords() const noexcept;
        [[nodiscard]] unsigned int getCount() const noexcept
        {
            return dbRecordsCount;
        }
        [[nodiscard]] std::string debugInfo() const override;

      private:
        std::vector<NotesRecord> records;
        unsigned int dbRecordsCount;
    };
} // namespace db::query
