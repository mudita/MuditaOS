// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryEventsEditICS.hpp"

namespace db::query::events
{
    EditICS::EditICS(EventsRecord record) : Query(Query::Type::Update), record(record)
    {}

    auto EditICS::getRecord() const -> EventsRecord
    {
        return record;
    }

    auto EditICS::debugInfo() const -> std::string
    {
        return "Edit";
    }

    EditICSResult::EditICSResult(bool ret) : ret(ret)
    {}

    auto EditICSResult::getResult() const -> bool
    {
        return ret;
    }

    auto EditICSResult::debugInfo() const -> std::string
    {
        return "EditResult";
    }
} // namespace db::query::events
