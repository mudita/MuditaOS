// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <includes/DBServiceName.hpp> // for db
#include <messages/QueryMessage.hpp>  // for QueryMessage
#include <Service/Message.hpp>        // for SendResult
#include <stdint.h>                   // for uint32_t
#include <utility>                    // for move
#include <memory>                     // for make_shared, allocator, unique_ptr

#include "DBServiceAPI.hpp"  // for DBServiceAPI
#include "BaseInterface.hpp" // for Interface, Interface::Name
#include "Service/Bus.hpp"   // for Bus

namespace db
{
    class Query;
} // namespace db
namespace sys
{
    class Service;
} // namespace sys

bool DBServiceAPI::GetQuery(sys::Service *serv, db::Interface::Name database, std::unique_ptr<db::Query> query)
{
    auto msg = std::make_shared<db::QueryMessage>(database, std::move(query));
    return sys::Bus::SendUnicast(msg, service::name::db, serv);
}

sys::SendResult DBServiceAPI::GetQueryWithReply(sys::Service *serv,
                                                db::Interface::Name database,
                                                std::unique_ptr<db::Query> query,
                                                std::uint32_t timeout)
{
    auto msg = std::make_shared<db::QueryMessage>(database, std::move(query));
    return sys::Bus::SendUnicast(msg, service::name::db, serv, timeout);
}
