// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmEventsAdd.hpp"

namespace db::query::alarmEvents
{
    Add::Add(const AlarmEventRecord &record) : Query(Query::Type::Create), record(record)
    {}

    auto Add::getRecord() const -> AlarmEventRecord
    {
        return record;
    }

    auto Add::debugInfo() const -> std::string
    {
        return std::string{"Add"};
    }

    AddResult::AddResult(bool ret) : ret(ret)
    {}

    auto AddResult::getResult() const -> bool
    {
        return ret;
    }

    auto AddResult::debugInfo() const -> std::string
    {
        return std::string{"AddResult"};
    }
} // namespace db::query::alarmEvents
