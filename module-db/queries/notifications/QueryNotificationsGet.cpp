#include "QueryNotificationsGet.hpp"

namespace db::query::notifications
{
    QueryGet::QueryGet(NotificationsRecord::Key key) : Query(Query::Type::Read), key(key)
    {}

    auto QueryGet::debugInfo() const -> std::string
    {
        return "QueryGet";
    }

    QueryGetResult::QueryGetResult(NotificationsRecord record) : record(record)
    {}

    auto QueryGetResult::getResult() const -> NotificationsRecord
    {
        return record;
    }

    auto QueryGetResult::debugInfo() const -> std::string
    {
        return "QueryGetResult";
    }
} // namespace db::query::notifications
