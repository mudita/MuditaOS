// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsGetNext.hpp"

#include <utility>

namespace db::query::alarmEvents
{
    GetNext::GetNext(TimePoint start, uint32_t offset, uint32_t limit)
        : Query(Query::Type::Read), start(start), offset(offset), limit(limit)
    {}

    auto GetNext::debugInfo() const -> std::string
    {
        return std::string{"GetNext"};
    }

    GetNextResult::GetNextResult(std::vector<AlarmEventRecord> records) : records(std::move(records))
    {}

    auto GetNextResult::getResult() const -> std::vector<AlarmEventRecord>
    {
        return records;
    }

    auto GetNextResult::debugInfo() const -> std::string
    {
        return std::string{"GetNextResult"};
    }
} // namespace db::query::alarmEvents
