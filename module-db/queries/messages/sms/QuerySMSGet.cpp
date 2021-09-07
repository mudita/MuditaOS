// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuerySMSGet.hpp"

#include <string>

using namespace db::query;

SMSGet::SMSGet(std::size_t limit, std::size_t offset) : RecordQuery(limit, offset)
{}

[[nodiscard]] auto SMSGet::debugInfo() const -> std::string
{
    return "SMSGet";
}

SMSGetWithTotalCount::SMSGetWithTotalCount(std::size_t limit, std::size_t offset) : SMSGet(limit, offset)
{}

[[nodiscard]] auto SMSGetWithTotalCount::debugInfo() const -> std::string
{
    return "SMSGetWithTotalCount";
}

SMSGetResult::SMSGetResult(std::vector<SMSRecord> records) : RecordQueryResult(std::move(records))
{}

[[nodiscard]] auto SMSGetResult::debugInfo() const -> std::string
{
    return "SMSGetResult";
}

SMSGetResultWithTotalCount::SMSGetResultWithTotalCount(std::vector<SMSRecord> records, std::size_t totalCount)
    : SMSGetResult(std::move(records)), totalCount(totalCount)
{}

auto SMSGetResultWithTotalCount::getTotalCount() const -> std::size_t
{
    return totalCount;
}

[[nodiscard]] auto SMSGetResultWithTotalCount::debugInfo() const -> std::string
{
    return "SMSGetResultWithTotalCount";
}
