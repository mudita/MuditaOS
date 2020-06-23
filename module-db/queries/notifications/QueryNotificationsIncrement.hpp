#pragma once

#include "module-db/Interface/NotificationsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class QueryIncrement : public Query
    {
      public:
        const NotificationsRecord::Key key;
        QueryIncrement(NotificationsRecord::Key key);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class QueryIncrementResult : public QueryResult
    {
        bool ret;

      public:
        QueryIncrementResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
