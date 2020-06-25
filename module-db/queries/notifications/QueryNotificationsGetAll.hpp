#pragma once

#include "module-db/Interface/NotificationsRecord.hpp"
#include <Common/Query.hpp>
#include <string>

namespace db::query::notifications
{
    class GetAll : public Query
    {
      public:
        GetAll();

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

    class GetAllResult : public QueryResult
    {
        std::unique_ptr<std::vector<NotificationsRecord>> records;

      public:
        GetAllResult(std::unique_ptr<std::vector<NotificationsRecord>> records);
        [[nodiscard]] auto getResult() -> std::unique_ptr<std::vector<NotificationsRecord>>;

        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::notifications
