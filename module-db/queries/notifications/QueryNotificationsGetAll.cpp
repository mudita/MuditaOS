// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QueryNotificationsGetAll.hpp"

namespace db::query::notifications
{
    GetAll::GetAll() : Query(Query::Type::Read)
    {}

    auto GetAll::debugInfo() const -> std::string
    {
        return "GetAll";
    }

    GetAllResult::GetAllResult(std::unique_ptr<std::vector<NotificationsRecord>> records) : records(std::move(records))
    {}

    auto GetAllResult::getResult() -> std::unique_ptr<std::vector<NotificationsRecord>>
    {
        return std::move(records);
    }

    auto GetAllResult::debugInfo() const -> std::string
    {
        return "GetAllResult";
    }
} // namespace db::query::notifications
