#include "QueryNotificationsClear.hpp"

namespace db::query::notifications
{
    QueryClear::QueryClear(NotificationsRecord::Key key) : Query(Query::Type::Update), key(key)
    {}

    auto QueryClear::debugInfo() const -> std::string
    {
        return "QueryClear";
    }

    QueryClearResult::QueryClearResult(bool ret) : ret(ret)
    {}

    auto QueryClearResult::getResult() const -> bool
    {
        return ret;
    }

    auto QueryClearResult::debugInfo() const -> std::string
    {
        return "QueryClearResult";
    }
} // namespace db::query::notifications
