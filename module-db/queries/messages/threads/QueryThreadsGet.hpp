// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/ThreadsTable.hpp>
#include "Interface/ThreadRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query
{

    class ThreadsGet : public Query
    {
      public:
        unsigned int offset;
        unsigned int limit;
        ThreadsGet(unsigned int offset, unsigned int limit);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ThreadsGetResults : public QueryResult
    {
        std::vector<ThreadRecord> results;

      public:
        ThreadsGetResults(std::vector<ThreadRecord> result_rows);
        [[nodiscard]] auto getResults() const -> std::vector<ThreadRecord>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
