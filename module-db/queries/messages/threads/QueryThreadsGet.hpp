#pragma once

#include <Tables/ThreadsTable.hpp>
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
        std::vector<ThreadsTableRow> results;

      public:
        ThreadsGetResults(std::vector<ThreadsTableRow> result_rows);
        [[nodiscard]] auto getResults() const -> std::vector<ThreadsTableRow>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
