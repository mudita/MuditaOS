// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmsSelectTurnedOn.hpp"

namespace db::query::alarms
{
    SelectTurnedOn::SelectTurnedOn() : Query(Query::Type::Read)
    {}

    auto SelectTurnedOn::debugInfo() const -> std::string
    {
        return std::string{"SelectTurnedOnAlarms"};
    }

    SelectTurnedOnResult::SelectTurnedOnResult(std::vector<AlarmsRecord> records) : records(std::move(records))
    {}

    auto SelectTurnedOnResult::getResult() -> std::vector<AlarmsRecord>
    {
        return records;
    }

    auto SelectTurnedOnResult::debugInfo() const -> std::string
    {
        return std::string{"SelectTurnedOnAlarmsResult"};
    }
} // namespace db::query::alarms
