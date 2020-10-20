// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryCalllogGet.hpp"

#include <string>

using namespace db::query;

CalllogGet::CalllogGet(std::size_t limit, std::size_t offset) : RecordQuery(limit, offset)
{}

CalllogGetResult::CalllogGetResult(const std::vector<CalllogRecord> &records) : RecordQueryResult(records)
{}

[[nodiscard]] auto CalllogGet::debugInfo() const -> std::string
{
    return "CalllogGet";
}

[[nodiscard]] auto CalllogGetResult::debugInfo() const -> std::string
{
    return "CalllogGetResult";
}
