// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsGetBetweenDates.hpp"

#include <utility>

namespace db::query::alarmEvents
{
    GetBetweenDates::GetBetweenDates(TimePoint start, TimePoint end, std::uint32_t offset, std::uint32_t limit)
        : Query(Query::Type::Read), start(start), end(end), offset(offset), limit(limit)
    {}

    auto GetBetweenDates::debugInfo() const -> std::string
    {
        return std::string{"GetBetweenDates"};
    }

    GetBetweenDatesResult::GetBetweenDatesResult(std::vector<AlarmEventRecord> records, std::uint32_t count)
        : records(std::move(records)), count(count)
    {}

    auto GetBetweenDatesResult::getResult() const -> std::pair<std::vector<AlarmEventRecord>, std::uint32_t>
    {
        return {records, count};
    }

    auto GetBetweenDatesResult::debugInfo() const -> std::string
    {
        return std::string{"GetBetweenDatesResult"};
    }
} // namespace db::query::alarmEvents
