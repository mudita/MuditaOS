// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNotificationsIncrement.hpp"

namespace db::query::notifications
{
    Increment::Increment(NotificationsRecord::Key key) : Query(Query::Type::Update), key(key)
    {}

    auto Increment::debugInfo() const -> std::string
    {
        return "Increment";
    }

    IncrementResult::IncrementResult(bool ret) : ret(ret)
    {}

    auto IncrementResult::getResult() const -> bool
    {
        return ret;
    }

    auto IncrementResult::debugInfo() const -> std::string
    {
        return "IncrementResult";
    }
} // namespace db::query::notifications
