// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmsGetLimited.hpp"

#include <utility>

namespace db::query::alarms
{
    GetLimited::GetLimited(uint32_t offset, uint32_t limit) : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto GetLimited::debugInfo() const -> std::string
    {
        return std::string{"GetLimited"};
    }

    GetLimitedResult::GetLimitedResult(std::vector<AlarmsRecord> records, uint32_t count)
        : records(std::move(records)), recordsCount(count)
    {}

    auto GetLimitedResult::getResult() -> std::vector<AlarmsRecord>
    {
        return records;
    }
    auto GetLimitedResult::getCountResult() -> uint32_t
    {
        return recordsCount;
    }

    auto GetLimitedResult::debugInfo() const -> std::string
    {
        return std::string{"GetLimitedResult"};
    }
} // namespace db::query::alarms
