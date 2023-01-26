// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNotificationsDecrement.hpp"

namespace db::query::notifications
{
    Decrement::Decrement(NotificationsRecord::Key key, size_t numberID, const size_t count)
        : Query(Query::Type::Update), key(key), count(count), numberID(numberID)
    {}

    auto Decrement::getKey() const noexcept -> NotificationsRecord::Key
    {
        return key;
    }
    auto Decrement::getCount() const noexcept -> size_t
    {
        return count;
    }

    auto Decrement::debugInfo() const -> std::string
    {
        return "Decrement";
    }
    auto Decrement::getNumberID() const noexcept -> size_t
    {
        return numberID;
    }

    DecrementResult::DecrementResult(bool ret) : ret(ret)
    {}

    auto DecrementResult::getResult() const noexcept -> bool
    {
        return ret;
    }

    auto DecrementResult::debugInfo() const -> std::string
    {
        return "DecrementResult";
    }
} // namespace db::query::notifications
