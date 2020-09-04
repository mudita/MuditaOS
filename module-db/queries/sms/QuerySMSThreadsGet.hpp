#pragma once

#include <Tables/ThreadsTable.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query
{

    class SMSThreadsGet : public Query
    {
      public:
        unsigned int offset;
        unsigned int limit;
        SMSThreadsGet(unsigned int offset, unsigned int limit);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class SMSThreadsGetResults : public QueryResult
    {
        std::vector<ThreadsTableRow> results;

      public:
        SMSThreadsGetResults(std::vector<ThreadsTableRow> result_rows);
        [[nodiscard]] auto getResults() const -> std::vector<ThreadsTableRow>;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

}; // namespace db::query
