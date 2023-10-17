// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/QueryMessage.hpp>

#include <BaseInterface.hpp>
#include <Common/Query.hpp>
#include <MessageType.hpp>

#include <memory>
#include <stdexcept>
#include <utility>

namespace db
{
    QueryMessage::QueryMessage(db::Interface::Name interface, std::unique_ptr<db::Query> query)
        : DBMessage(MessageType::DBQuery), query(std::move(query)), interface(interface)
    {}

    db::Interface::Name QueryMessage::getInterface() const
    {
        return interface;
    }

    auto QueryMessage::getQuery() -> std::unique_ptr<db::Query>
    {
        if (query == nullptr) {
            throw std::runtime_error("Invalid query (already moved from)");
        }

        return std::move(query);
    }

    QueryResponse::QueryResponse(std::unique_ptr<db::QueryResult> result)
        : DBResponseMessage(0, 0, MessageType::DBQuery), result(std::move(result))
    {}

    auto QueryResponse::getResult() -> std::unique_ptr<db::QueryResult>
    {
        return std::move(result);
    }
} // namespace db
