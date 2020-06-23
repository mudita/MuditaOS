#pragma once

#include "module-db/Interface/NotificationsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class QueryGet : public Query
    {
      public:
        const NotificationsRecord::Key key;
        QueryGet(NotificationsRecord::Key key);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class QueryGetResult : public QueryResult
    {
        NotificationsRecord record;

      public:
        QueryGetResult(NotificationsRecord record);
        [[nodiscard]] auto getResult() const -> NotificationsRecord;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
