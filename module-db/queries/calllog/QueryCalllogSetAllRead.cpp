// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QueryCalllogSetAllRead.hpp"

namespace db::query::calllog
{
    SetAllRead::SetAllRead() : Query(Query::Type::Update)
    {}

    auto SetAllRead::debugInfo() const -> std::string
    {
        return "SetAllRead";
    }

    SetAllReadResult::SetAllReadResult(bool ret) : ret(ret)
    {}

    [[nodiscard]] auto SetAllReadResult::debugInfo() const -> std::string
    {
        return "SetAllReadResult";
    }
} // namespace db::query::calllog
