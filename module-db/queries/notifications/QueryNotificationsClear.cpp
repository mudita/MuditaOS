#include "QueryNotificationsClear.hpp"

namespace db::query::notifications
{
    Clear::Clear(NotificationsRecord::Key key) : Query(Query::Type::Update), key(key)
    {}

    auto Clear::debugInfo() const -> std::string
    {
        return "Clear";
    }

    ClearResult::ClearResult(bool ret) : ret(ret)
    {}

    auto ClearResult::getResult() const -> bool
    {
        return ret;
    }

    auto ClearResult::debugInfo() const -> std::string
    {
        return "ClearResult";
    }
} // namespace db::query::notifications
