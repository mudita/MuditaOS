// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryAlarmsEdit.hpp"

namespace db::query::alarms
{
    Edit::Edit(const AlarmsRecord &record) : Query(Query::Type::Update), record(record)
    {}

    auto Edit::getRecord() const -> AlarmsRecord
    {
        return record;
    }

    auto Edit::debugInfo() const -> std::string
    {
        return std::string{"Edit"};
    }

    EditResult::EditResult(bool ret) : ret(ret)
    {}

    auto EditResult::succeed() const -> bool
    {
        return ret;
    }

    auto EditResult::debugInfo() const -> std::string
    {
        return std::string{"EditResult"};
    }
} // namespace db::query::alarms
