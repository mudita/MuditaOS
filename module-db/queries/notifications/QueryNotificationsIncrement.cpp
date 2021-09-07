// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNotificationsIncrement.hpp"

namespace db::query::notifications
{
    Increment::Increment(NotificationsRecord::Key key, const utils::PhoneNumber::View &number)
        : Query(Query::Type::Update), key(key), number(number)
    {}

    auto Increment::getKey() const noexcept -> NotificationsRecord::Key
    {
        return key;
    }
    auto Increment::getNumber() const noexcept -> const utils::PhoneNumber::View &
    {
        return number;
    }

    auto Increment::debugInfo() const -> std::string
    {
        return "Increment";
    }

    IncrementResult::IncrementResult(bool ret) : ret(ret)
    {}

    auto IncrementResult::getResult() const noexcept -> bool
    {
        return ret;
    }

    auto IncrementResult::debugInfo() const -> std::string
    {
        return "IncrementResult";
    }
} // namespace db::query::notifications
