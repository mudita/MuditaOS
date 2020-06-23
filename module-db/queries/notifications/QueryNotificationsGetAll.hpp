#pragma once

#include "module-db/Interface/NotificationsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class QueryGetAll : public Query
    {
      public:
        QueryGetAll();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class QueryGetAllResult : public QueryResult
    {
        std::unique_ptr<std::vector<NotificationsRecord>> records;

      public:
        QueryGetAllResult(std::unique_ptr<std::vector<NotificationsRecord>> records);
        [[nodiscard]] auto getResult() -> std::unique_ptr<std::vector<NotificationsRecord>>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
