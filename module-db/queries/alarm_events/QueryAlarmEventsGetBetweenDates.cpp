// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsGetBetweenDates.hpp"

#include <utility>

namespace db::query::alarmEvents
{
    GetBetweenDates::GetBetweenDates(TimePoint start, TimePoint end, uint32_t offset, uint32_t limit)
        : Query(Query::Type::Read), start(start), end(end), offset(offset), limit(limit)
    {}

    auto GetBetweenDates::debugInfo() const -> std::string
    {
        return std::string{"GetBetweenDates"};
    }

    GetBetweenDatesResult::GetBetweenDatesResult(std::vector<AlarmEventRecord> records) : records(std::move(records))
    {}

    auto GetBetweenDatesResult::getResult() const -> std::vector<AlarmEventRecord>
    {
        return records;
    }

    auto GetBetweenDatesResult::debugInfo() const -> std::string
    {
        return std::string{"GetBetweenDatesResult"};
    }
} // namespace db::query::alarmEvents
