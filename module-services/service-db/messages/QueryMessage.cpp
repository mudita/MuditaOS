#include "QueryMessage.hpp"

namespace db
{
    QueryMessage::QueryMessage(db::DB database, std::unique_ptr<db::Query> query)
        : DBMessage(MessageType::DBQuery), query(std::move(query)), database(database)
    {}

    DB QueryMessage::getDatabase() const
    {
        return database;
    }
} // namespace db
