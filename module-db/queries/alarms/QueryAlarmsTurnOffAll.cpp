// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmsTurnOffAll.hpp"

namespace db::query::alarms
{
    TurnOffAll::TurnOffAll() : Query(Query::Type::Update)
    {}

    auto TurnOffAll::debugInfo() const -> std::string
    {
        return std::string{"TurnOffAll"};
    }

    TurnOffAllResult::TurnOffAllResult(bool ret) : ret(ret)
    {}

    [[nodiscard]] auto TurnOffAllResult::debugInfo() const -> std::string
    {
        return std::string{"TurnOffAllResult"};
    }
} // namespace db::query::alarms
