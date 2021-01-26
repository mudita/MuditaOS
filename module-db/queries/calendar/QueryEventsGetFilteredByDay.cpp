// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryEventsGetFilteredByDay.hpp"

namespace db::query::events
{
    GetFilteredByDay::GetFilteredByDay(utils::time::TimePoint filterDate, uint32_t offset, uint32_t limit)
        : Query(Query::Type::Read), filterDate(filterDate), offset(offset), limit(limit)
    {}

    auto GetFilteredByDay::debugInfo() const -> std::string
    {
        return "GetFilteredByDay";
    }

    GetFilteredByDayResult::GetFilteredByDayResult(const std::vector<EventsRecord> &records, uint32_t count)
        : records{std::move(records)}, recordsCount{count}
    {}

    auto GetFilteredByDayResult::getResult() -> std::vector<EventsRecord>
    {
        return records;
    }

    auto GetFilteredByDayResult::getCountResult() -> uint32_t
    {
        return recordsCount;
    }

    auto GetFilteredByDayResult::debugInfo() const -> std::string
    {
        return "GetFilteredByDayResult";
    }
} // namespace db::query::events
