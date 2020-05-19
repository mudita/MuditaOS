#pragma once

#include "DBMessage.hpp"
#include <module-db/Common/Query.hpp>
#include <includes/interfaces.hpp>

namespace db
{
    class QueryMessage : public DBMessage
    {
        std::unique_ptr<db::Query> query;
        db::interface interface;

      public:
        QueryMessage(db::interface interface, std::unique_ptr<db::Query> query);
        [[nodiscard]] auto getInterface() const -> db::interface;
    };
} // namespace db
