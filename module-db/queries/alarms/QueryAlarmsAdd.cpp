// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmsAdd.hpp"

namespace db::query::alarms
{
    Add::Add(const AlarmsRecord &record) : Query(Query::Type::Create), record(record)
    {}

    auto Add::getRecord() const -> AlarmsRecord
    {
        return record;
    }

    auto Add::debugInfo() const -> std::string
    {
        return std::string{"Add"};
    }

    AddResult::AddResult(bool ret) : ret(ret)
    {}

    auto AddResult::succeed() const -> bool
    {
        return ret;
    }

    auto AddResult::debugInfo() const -> std::string
    {
        return std::string{"AddResult"};
    }
} // namespace db::query::alarms
