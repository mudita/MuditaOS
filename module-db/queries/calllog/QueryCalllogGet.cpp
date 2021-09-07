// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryCalllogGet.hpp"

#include <string>

using namespace db::query;

CalllogGet::CalllogGet(std::size_t limit, std::size_t offset) : RecordQuery(limit, offset)
{}

[[nodiscard]] auto CalllogGet::debugInfo() const -> std::string
{
    return "CalllogGet";
}

CalllogGetResult::CalllogGetResult(std::vector<CalllogRecord> &&records, unsigned int dbRecordsCount)
    : RecordQueryResult(std::move(records)), dbRecordsCount{dbRecordsCount}
{}

[[nodiscard]] auto CalllogGetResult::debugInfo() const -> std::string
{
    return "CalllogGetResult";
}

auto CalllogGetResult::getTotalCount() const noexcept -> unsigned int
{
    return dbRecordsCount;
}
