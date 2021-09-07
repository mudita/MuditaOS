// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/NotificationsRecord.hpp>
#include <Common/Query.hpp>
#include <string>
#include <PhoneNumber.hpp>

namespace db::query::notifications
{
    class Increment : public Query
    {
        const NotificationsRecord::Key key;
        const utils::PhoneNumber::View number;

      public:
        Increment(NotificationsRecord::Key key, const utils::PhoneNumber::View &number);

        [[nodiscard]] auto getKey() const noexcept -> NotificationsRecord::Key;
        [[nodiscard]] auto getNumber() const noexcept -> const utils::PhoneNumber::View &;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class IncrementResult : public QueryResult
    {
        bool ret;

      public:
        explicit IncrementResult(bool ret);
        [[nodiscard]] auto getResult() const noexcept -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
