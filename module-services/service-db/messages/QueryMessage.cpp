#include "QueryMessage.hpp"

namespace db
{
    QueryMessage::QueryMessage(db::interface interface, std::unique_ptr<db::Query> query)
        : DBMessage(MessageType::DBQuery), query(std::move(query)), interface(interface)
    {}

    interface QueryMessage::getInterface() const
    {
        return interface;
    }
} // namespace db
