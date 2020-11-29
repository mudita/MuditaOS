// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmsSelectFirstUpcoming.hpp"

namespace db::query::alarms
{
    SelectFirstUpcoming::SelectFirstUpcoming(TimePoint filter_from, TimePoint filter_till)
        : Query(Query::Type::Read), filter_from(filter_from), filter_till(filter_till)
    {}

    auto SelectFirstUpcoming::debugInfo() const -> std::string
    {
        return "SelectFirstUpcomingAlarms";
    }

    SelectFirstUpcomingResult::SelectFirstUpcomingResult(std::unique_ptr<std::vector<AlarmsRecord>> records)
        : records(std::move(records))
    {}

    auto SelectFirstUpcomingResult::getResult() -> std::unique_ptr<std::vector<AlarmsRecord>>
    {
        return std::move(records);
    }

    auto SelectFirstUpcomingResult::debugInfo() const -> std::string
    {
        return "SelectFirstUpcomingAlarmsResult";
    }
} // namespace db::query::alarms
