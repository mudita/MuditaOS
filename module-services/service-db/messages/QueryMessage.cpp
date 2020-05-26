#include "QueryMessage.hpp"

namespace db
{
    QueryMessage::QueryMessage(db::Interface::Name interface, std::unique_ptr<db::Query> query)
        : DBMessage(MessageType::DBQuery), query(std::move(query)), interface(interface)
    {}

    db::Interface::Name QueryMessage::getInterface() const
    {
        return interface;
    }

    auto QueryMessage::getQuery() const -> db::Query *
    {
        return query.get();
    }

    QueryResponse::QueryResponse(std::unique_ptr<db::QueryResult> result)
        : DBResponseMessage(0, 0, MessageType::DBQuery), result(std::move(result))
    {}

    auto QueryResponse::getResult() const -> db::QueryResult *
    {
        return result.get();
    }
} // namespace db
