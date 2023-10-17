// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/NotificationsRecord.hpp>
#include <Common/Query.hpp>
#include <string>
#include <PhoneNumber.hpp>

namespace db::query::notifications
{
    class MultipleIncrement : public Query
    {
        const NotificationsRecord::Key key;
        const std::vector<utils::PhoneNumber::View> numbers;

      public:
        MultipleIncrement(NotificationsRecord::Key key, const std::vector<utils::PhoneNumber::View> &numbers);

        [[nodiscard]] auto getKey() const noexcept -> NotificationsRecord::Key;
        [[nodiscard]] auto getNumbers() const noexcept -> const std::vector<utils::PhoneNumber::View> &;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class MultipleIncrementResult : public QueryResult
    {
        bool ret;

      public:
        explicit MultipleIncrementResult(bool ret);
        [[nodiscard]] auto getResult() const noexcept -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
