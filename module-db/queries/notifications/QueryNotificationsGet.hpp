// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Interface/NotificationsRecord.hpp>
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class Get : public Query
    {
        const NotificationsRecord::Key key;

      public:
        explicit Get(NotificationsRecord::Key key);

        [[nodiscard]] auto getKey() const noexcept -> NotificationsRecord::Key;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetResult : public QueryResult
    {
        NotificationsRecord record;

      public:
        explicit GetResult(NotificationsRecord record);
        [[nodiscard]] auto getResult() const -> NotificationsRecord;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
