// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/NotificationsRecord.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class Clear : public Query
    {
        const NotificationsRecord::Key key;

      public:
        explicit Clear(NotificationsRecord::Key key);

        [[nodiscard]] auto getKey() const noexcept -> NotificationsRecord::Key;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ClearResult : public QueryResult
    {
        bool ret;

      public:
        explicit ClearResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
