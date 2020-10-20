// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/NotificationsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class Get : public Query
    {
      public:
        const NotificationsRecord::Key key;
        Get(NotificationsRecord::Key key);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetResult : public QueryResult
    {
        NotificationsRecord record;

      public:
        GetResult(NotificationsRecord record);
        [[nodiscard]] auto getResult() const -> NotificationsRecord;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
