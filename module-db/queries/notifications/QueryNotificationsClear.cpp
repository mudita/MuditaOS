// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNotificationsClear.hpp"

namespace db::query::notifications
{
    Clear::Clear(NotificationsRecord::Key key) : Query(Query::Type::Update), key(key)
    {}

    auto Clear::debugInfo() const -> std::string
    {
        return "Clear";
    }

    auto Clear::getKey() const noexcept -> NotificationsRecord::Key
    {
        return key;
    }

    ClearResult::ClearResult(bool ret) : ret(ret)
    {}

    auto ClearResult::getResult() const -> bool
    {
        return ret;
    }

    auto ClearResult::debugInfo() const -> std::string
    {
        return "ClearResult";
    }
} // namespace db::query::notifications
