// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-db/DBServiceAPI.hpp"
#include "service-db/DBServiceName.hpp"
#include "service-db/QueryMessage.hpp"

#include <BaseInterface.hpp>
#include <Service/Bus.hpp>
#include <Service/Message.hpp>

#include <cstdint>
#include <memory>
#include <utility>

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
