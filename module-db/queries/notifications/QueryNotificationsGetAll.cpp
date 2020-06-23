#include "QueryNotificationsGetAll.hpp"

namespace db::query::notifications
{
    QueryGetAll::QueryGetAll() : Query(Query::Type::Read)
    {}

    auto QueryGetAll::debugInfo() const -> std::string
    {
        return "QueryGetAll";
    }

    QueryGetAllResult::QueryGetAllResult(std::unique_ptr<std::vector<NotificationsRecord>> records)
        : records(std::move(records))
    {}

    auto QueryGetAllResult::getResult() -> std::unique_ptr<std::vector<NotificationsRecord>>
    {
        return std::move(records);
    }

    auto QueryGetAllResult::debugInfo() const -> std::string
    {
        return "QueryGetAllResult";
    }
} // namespace db::query::notifications
