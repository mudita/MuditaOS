// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryEventsSelectFirstUpcoming.hpp"

namespace db::query::events
{
    SelectFirstUpcoming::SelectFirstUpcoming(TimePoint filter_from, TimePoint filter_till)
        : Query(Query::Type::Read), filter_from(filter_from), filter_till(filter_till)
    {}

    auto SelectFirstUpcoming::debugInfo() const -> std::string
    {
        return "SelectFirstUpcoming";
    }

    SelectFirstUpcomingResult::SelectFirstUpcomingResult(std::vector<EventsRecord> records)
        : records{std::move(records)}
    {}

    auto SelectFirstUpcomingResult::getResult() -> std::vector<EventsRecord>
    {
        return records;
    }

    auto SelectFirstUpcomingResult::debugInfo() const -> std::string
    {
        return "SelectFirstUpcomingResult";
    }
} // namespace db::query::events
