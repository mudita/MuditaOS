// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query
{
    /// implements search for SMS by text
    class ThreadsSearch : public Query
    {
      public:
        std::string text;
        unsigned int startingPosition;
        unsigned int depth;
        ThreadsSearch(std::string text_to_search, unsigned int starting_position, unsigned int depth);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ThreadsSearchResult : public QueryResult
    {
        unsigned int results_max_depth = 0;
        std::vector<ThreadsTableRow> results;

      public:
        ThreadsSearchResult(unsigned int results_max_depth, std::vector<ThreadsTableRow> result_rows);
        [[nodiscard]] auto getMax() const -> unsigned int;
        [[nodiscard]] auto getResults() const -> std::vector<ThreadsTableRow>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
