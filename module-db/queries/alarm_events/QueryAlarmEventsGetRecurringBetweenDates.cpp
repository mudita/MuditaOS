// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsGetRecurringBetweenDates.hpp"

#include <utility>

namespace db::query::alarmEvents
{
    GetRecurringBetweenDates::GetRecurringBetweenDates(TimePoint start, TimePoint end, uint32_t offset, uint32_t limit)
        : Query(Query::Type::Read), start(start), end(end), offset(offset), limit(limit)
    {}

    auto GetRecurringBetweenDates::debugInfo() const -> std::string
    {
        return std::string{"GetRecurringBetweenDates"};
    }

    GetRecurringBetweenDatesResult::GetRecurringBetweenDatesResult(std::vector<AlarmEventRecord> records)
        : records(std::move(records))
    {}

    auto GetRecurringBetweenDatesResult::getResult() const -> std::vector<AlarmEventRecord>
    {
        return records;
    }

    auto GetRecurringBetweenDatesResult::debugInfo() const -> std::string
    {
        return std::string{"GetRecurringBetweenDatesResult"};
    }
} // namespace db::query::alarmEvents
