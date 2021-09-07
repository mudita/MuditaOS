// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsGet.hpp"

namespace db::query::alarmEvents
{
    Get::Get(uint32_t id) : Query(Query::Type::Read), id(id)
    {}

    auto Get::debugInfo() const -> std::string
    {
        return std::string{"Get"};
    }

    GetResult::GetResult(const AlarmEventRecord &record) : record(record)
    {}

    auto GetResult::getResult() const -> AlarmEventRecord
    {
        return record;
    }

    auto GetResult::debugInfo() const -> std::string
    {
        return std::string{"GetResult"};
    }
} // namespace db::query::alarmEvents
