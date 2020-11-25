// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryEventsRemoveICS.hpp"

namespace db::query::events
{
    RemoveICS::RemoveICS(const std::string &UID) : Query(Query::Type::Delete), UID(UID)
    {}

    auto RemoveICS::debugInfo() const -> std::string
    {
        return "Remove";
    }

    RemoveICSResult::RemoveICSResult(bool ret) : ret(ret)
    {}

    auto RemoveICSResult::getResult() const -> bool
    {
        return ret;
    }

    auto RemoveICSResult::debugInfo() const -> std::string
    {
        return "RemoveResult";
    }
} // namespace db::query::events
