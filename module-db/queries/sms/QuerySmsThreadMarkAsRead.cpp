#include "QuerySmsThreadMarkAsRead.hpp"

namespace db::query::smsthread
{
    MarkAsRead::MarkAsRead(uint32_t id, Read read) : Query(Query::Type::Update), read(read), id(id)
    {}

    auto MarkAsRead::debugInfo() const -> std::string
    {
        return "MarkAsRead";
    }

    MarkAsReadResult::MarkAsReadResult(bool ret) : ret(ret)
    {}

    auto MarkAsReadResult::getResult() const -> bool
    {
        return ret;
    }

    auto MarkAsReadResult::debugInfo() const -> std::string
    {
        return "MarkAsReadResult";
    }
} // namespace db::query::smsthread
