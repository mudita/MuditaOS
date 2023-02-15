// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/NotificationsRecord.hpp>
#include <Common/Query.hpp>
#include <string>
#include <PhoneNumber.hpp>
#include <cstdint>

namespace db::query::notifications
{
    class Decrement : public Query
    {
        const NotificationsRecord::Key key;
        const size_t count;

      public:
        Decrement(NotificationsRecord::Key key, const size_t count);

        [[nodiscard]] auto getKey() const noexcept -> NotificationsRecord::Key;
        [[nodiscard]] auto getCount() const noexcept -> size_t;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class DecrementResult : public QueryResult
    {
        bool ret;

      public:
        explicit DecrementResult(bool ret);
        [[nodiscard]] auto getResult() const noexcept -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
