// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsGetLimited.hpp"

#include <utility>

namespace db::query::alarmEvents
{
    GetLimited::GetLimited(uint32_t offset, uint32_t limit) : Query(Query::Type::Read), offset(offset), limit(limit)
    {}

    auto GetLimited::debugInfo() const -> std::string
    {
        return std::string{"GetLimited"};
    }

    GetLimitedResult::GetLimitedResult(std::vector<AlarmEventRecord> records) : records(std::move(records))
    {}

    auto GetLimitedResult::getResult() const -> std::vector<AlarmEventRecord>
    {
        return records;
    }

    auto GetLimitedResult::debugInfo() const -> std::string
    {
        return std::string{"GetLimitedResult"};
    }
} // namespace db::query::alarmEvents
