#include "QueryNotificationsIncrement.hpp"

namespace db::query::notifications
{
    QueryIncrement::QueryIncrement(NotificationsRecord::Key key) : Query(Query::Type::Update), key(key)
    {}

    auto QueryIncrement::debugInfo() const -> std::string
    {
        return "QueryIncrement";
    }

    QueryIncrementResult::QueryIncrementResult(bool ret) : ret(ret)
    {}

    auto QueryIncrementResult::getResult() const -> bool
    {
        return ret;
    }

    auto QueryIncrementResult::debugInfo() const -> std::string
    {
        return "QueryIncrementResult";
    }
} // namespace db::query::notifications
