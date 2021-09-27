// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsToggleAll.hpp"

namespace db::query::alarmEvents
{
    ToggleAll::ToggleAll(const bool toggle) : Query(Query::Type::Update), toggle(toggle)
    {}

    auto ToggleAll::debugInfo() const -> std::string
    {
        return std::string{"ToggleAllResult"};
    }

    ToggleAllResult::ToggleAllResult(const bool ret) : ret(ret)
    {}

    auto ToggleAllResult::getResult() const -> bool
    {
        return ret;
    }

    auto ToggleAllResult::debugInfo() const -> std::string
    {
        return std::string{"ToggleAllResult"};
    }
} // namespace db::query::alarmEvents
