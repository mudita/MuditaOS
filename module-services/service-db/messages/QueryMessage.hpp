#pragma once

#include "DBMessage.hpp"
#include <module-db/Common/Query.hpp>
#include <includes/databases.hpp>

namespace db
{
    class QueryMessage : public DBMessage
    {
        std::unique_ptr<db::Query> query;
        db::DB database;

      public:
        QueryMessage(db::DB database, std::unique_ptr<db::Query> query);
        [[nodiscard]] auto getDatabase() const -> db::DB;
    };
} // namespace db
