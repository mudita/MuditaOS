// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
        explicit QueryNotesGetByText(std::string text);

        [[nodiscard]] const std::string &getText() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        std::string text;
    };

    class NotesGetByTextResult : public QueryResult
    {
      public:
        explicit NotesGetByTextResult(std::vector<NotesRecord> notes);

        [[nodiscard]] const std::vector<NotesRecord> &getRecords() const noexcept;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        std::vector<NotesRecord> records;
    };
} // namespace db::query
