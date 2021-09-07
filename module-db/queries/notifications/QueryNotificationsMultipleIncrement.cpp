// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QueryNotificationsMultipleIncrement.hpp"

namespace db::query::notifications
{
    MultipleIncrement::MultipleIncrement(NotificationsRecord::Key key,
                                         const std::vector<utils::PhoneNumber::View> &numbers)
        : Query(Query::Type::Update), key(key), numbers(numbers)
    {}

    auto MultipleIncrement::getKey() const noexcept -> NotificationsRecord::Key
    {
        return key;
    }
    auto MultipleIncrement::getNumbers() const noexcept -> const std::vector<utils::PhoneNumber::View> &
    {
        return numbers;
    }

    auto MultipleIncrement::debugInfo() const -> std::string
    {
        return "MultipleIncrement";
    }

    MultipleIncrementResult::MultipleIncrementResult(bool ret) : ret(ret)
    {}

    auto MultipleIncrementResult::getResult() const noexcept -> bool
    {
        return ret;
    }

    auto MultipleIncrementResult::debugInfo() const -> std::string
    {
        return "MultipleIncrementResult";
    }
} // namespace db::query::notifications
