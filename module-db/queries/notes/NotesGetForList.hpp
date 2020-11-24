// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <Common/Query.hpp>
#include "Interface/NotesRecord.hpp"

namespace db::query
{
    class NotesGetForList : public Query
    {
      public:
        const unsigned int offset;
        const unsigned int limit;

        explicit NotesGetForList(unsigned int offset = 0U, unsigned int limit = 0U);
        [[nodiscard]] std::string debugInfo() const override;
    };

    class NotesGetForListResult : public QueryResult
    {
      public:
        explicit NotesGetForListResult(std::vector<NotesRecord> notes);

        [[nodiscard]] const std::vector<NotesRecord> &getRecords() const;
        [[nodiscard]] std::string debugInfo() const override;

      private:
        std::vector<NotesRecord> records;
    };
} // namespace db::query
