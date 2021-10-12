// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsGetInRange.hpp"

#include <utility>

namespace db::query::alarmEvents
{
    GetInRange::GetInRange(uint32_t offset, uint32_t limit) : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto GetInRange::debugInfo() const -> std::string
    {
        return std::string{"GetInRange"};
    }

    GetInRangeResult::GetInRangeResult(std::vector<AlarmEventRecord> records, std::uint32_t count)
        : records(std::move(records)), count(count)
    {}

    auto GetInRangeResult::getResult() const -> std::pair<std::vector<AlarmEventRecord>, std::uint32_t>
    {
        return {records, count};
    }

    auto GetInRangeResult::debugInfo() const -> std::string
    {
        return std::string{"GetInRangeResult"};
    }
} // namespace db::query::alarmEvents
