#pragma once

#include "module-db/Interface/NotificationsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class Clear : public Query
    {
      public:
        const NotificationsRecord::Key key;
        Clear(NotificationsRecord::Key key);

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class ClearResult : public QueryResult
    {
        bool ret;

      public:
        ClearResult(bool ret);
        [[nodiscard]] auto getResult() const -> bool;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
