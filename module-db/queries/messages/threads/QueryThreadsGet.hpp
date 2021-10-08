// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

    class ThreadsGetWithTotalCount : public ThreadsGet
    {
      public:
        ThreadsGetWithTotalCount(std::size_t offset, std::size_t limit);
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

    class ThreadsGetResultsWithTotalCount : public ThreadsGetResults
    {
      public:
        ThreadsGetResultsWithTotalCount(std::vector<ThreadRecord> records, std::size_t allLength);
        [[nodiscard]] auto debugInfo() const -> std::string override;
        auto getTotalCount() const noexcept -> std::size_t;

      private:
        std::size_t totalCount;
    };
}; // namespace db::query
