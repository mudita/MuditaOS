#pragma once

#include "DBMessage.hpp"
#include <module-db/Common/Query.hpp>
#include <module-db/Interface/BaseInterface.hpp>

namespace db
{
    class QueryMessage : public DBMessage
    {
        std::unique_ptr<db::Query> query;
        db::Interface::Name interface;

      public:
        QueryMessage(db::Interface::Name interface, std::unique_ptr<db::Query> query);
        [[nodiscard]] auto getInterface() const -> db::Interface::Name;
        [[nodiscard]] auto getQuery() const -> db::Query *;
    };

    ///@note please see that this message might carry more information than just result
    /// it might be useful to tell to what query response that is - not needed now, so not added
    class QueryResponse : public DBResponseMessage
    {
        std::unique_ptr<db::QueryResult> result;

      public:
        QueryResponse(std::unique_ptr<db::QueryResult> result);
        [[nodiscard]] auto getResult() const -> db::QueryResult *;
    };
} // namespace db
