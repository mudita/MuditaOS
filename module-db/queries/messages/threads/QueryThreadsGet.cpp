// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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

    ThreadsGetResults::ThreadsGetResults(std::vector<ThreadsTableRow> result_rows) : results(std::move(result_rows))
    {}

    auto ThreadsGetResults::getResults() const -> std::vector<ThreadsTableRow>
    {
        return results;
    }

    [[nodiscard]] auto ThreadsGetResults::debugInfo() const -> std::string
    {
        return "SMSThreadsGetResults";
    }
} // namespace db::query
