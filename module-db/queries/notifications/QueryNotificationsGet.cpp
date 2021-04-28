// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNotificationsGet.hpp"

namespace db::query::notifications
{
    Get::Get(NotificationsRecord::Key key) : Query(Query::Type::Read), key(key)
    {}

    auto Get::debugInfo() const -> std::string
    {
        return "Get";
    }

    auto Get::getKey() const noexcept -> NotificationsRecord::Key
    {
        return key;
    }

    GetResult::GetResult(NotificationsRecord record) : record(std::move(record))
    {}

    auto GetResult::getResult() const -> NotificationsRecord
    {
        return record;
    }

    auto GetResult::debugInfo() const -> std::string
    {
        return "GetResult";
    }
} // namespace db::query::notifications
