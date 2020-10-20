// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Interface/NotificationsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class Increment : public Query
    {
      public:
        const NotificationsRecord::Key key;
        Increment(NotificationsRecord::Key key);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class IncrementResult : public QueryResult
    {
        bool ret;

      public:
        IncrementResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
