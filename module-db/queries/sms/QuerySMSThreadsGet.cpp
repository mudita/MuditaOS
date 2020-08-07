#include <Tables/ThreadsTable.hpp>
#include <utility>
#include "QuerySMSThreadsGet.hpp"

namespace db::query
{
    SMSThreadsGet::SMSThreadsGet(unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto SMSThreadsGet::debugInfo() const -> std::string
    {
        return "SMSThreadsGet";
    }

    SMSThreadsGetResults::SMSThreadsGetResults(std::vector<ThreadsTableRow> result_rows)
        : results(std::move(result_rows))
    {}

    auto SMSThreadsGetResults::getResults() const -> std::vector<ThreadsTableRow>
    {
        return results;
    }

    [[nodiscard]] auto SMSThreadsGetResults::debugInfo() const -> std::string
    {
        return "SMSThreadsGetResults";
    }
} // namespace db::query
