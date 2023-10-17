// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Tables/ThreadsTable.hpp>
#include <utility>
#include "QueryThreadsGet.hpp"

namespace db::query
{
    ThreadsGet::ThreadsGet(unsigned int offset, unsigned int limit)
        : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto ThreadsGet::debugInfo() const -> std::string
    {
        return "SMSThreadsGet";
    }

    ThreadsGetWithTotalCount::ThreadsGetWithTotalCount(std::size_t offset, std::size_t limit)
        : ThreadsGet(offset, limit)
    {}

    [[nodiscard]] auto ThreadsGetWithTotalCount::debugInfo() const -> std::string
    {
        return "ThreadsGetWithTotalCount";
    }

    ThreadsGetResults::ThreadsGetResults(std::vector<ThreadRecord> result_rows) : results(std::move(result_rows))
    {}

    auto ThreadsGetResults::getResults() const -> std::vector<ThreadRecord>
    {
        return results;
    }

    [[nodiscard]] auto ThreadsGetResults::debugInfo() const -> std::string
    {
        return "SMSThreadsGetResults";
    }

    ThreadsGetResultsWithTotalCount::ThreadsGetResultsWithTotalCount(std::vector<ThreadRecord> records,
                                                                     std::size_t totalCount)
        : ThreadsGetResults(std::move(records)), totalCount(totalCount)
    {}

    auto ThreadsGetResultsWithTotalCount::getTotalCount() const noexcept -> std::size_t
    {
        return totalCount;
    }

    [[nodiscard]] auto ThreadsGetResultsWithTotalCount::debugInfo() const -> std::string
    {
        return "ThreadsGetResultsWithTotalCount";
    }
} // namespace db::query
