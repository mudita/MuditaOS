// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmsGet.hpp"

namespace db::query::alarms
{
    Get::Get(uint32_t id) : Query(Query::Type::Read), id(id)
    {}

    auto Get::debugInfo() const -> std::string
    {
        return std::string{"Get"};
    }

    GetResult::GetResult(const AlarmsRecord &record) : record(record)
    {}

    auto GetResult::getResult() const -> AlarmsRecord
    {
        return record;
    }

    auto GetResult::debugInfo() const -> std::string
    {
        return std::string{"GetResult"};
    }
} // namespace db::query::alarms
