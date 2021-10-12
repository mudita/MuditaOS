// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsGetEnabled.hpp"

#include <utility>

namespace db::query::alarmEvents
{
    GetEnabled::GetEnabled() : Query(Query::Type::Read)
    {}

    auto GetEnabled::debugInfo() const -> std::string
    {
        return std::string{"GetEnabled"};
    }

    GetEnabledResult::GetEnabledResult(std::vector<AlarmEventRecord> records) : records(std::move(records))
    {}

    auto GetEnabledResult::getResult() const -> std::vector<AlarmEventRecord>
    {
        return records;
    }

    auto GetEnabledResult::debugInfo() const -> std::string
    {
        return std::string{"GetEnabledResult"};
    }
} // namespace db::query::alarmEvents
