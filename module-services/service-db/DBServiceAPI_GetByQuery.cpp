// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-db/DBServiceAPI.hpp"
#include "service-db/DBServiceName.hpp"
#include "service-db/QueryMessage.hpp"

#include <BaseInterface.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>

#include <cstdint>
#include <memory>
#include <utility>

namespace db
{
    class Query;
} // namespace db

std::pair<bool, std::uint64_t> DBServiceAPI::GetQuery(sys::Service *serv,
                                                      db::Interface::Name database,
                                                      std::unique_ptr<db::Query> query)
{
    auto msg             = std::make_shared<db::QueryMessage>(database, std::move(query));
    const auto isSuccess = serv->bus.sendUnicast(msg, service::name::db);
    return std::make_pair(isSuccess, msg->uniID);
}

sys::SendResult DBServiceAPI::GetQueryWithReply(sys::Service *serv,
                                                db::Interface::Name database,
                                                std::unique_ptr<db::Query> query,
                                                std::uint32_t timeout)
{
    auto msg = std::make_shared<db::QueryMessage>(database, std::move(query));
    return serv->bus.sendUnicastSync(msg, service::name::db, timeout);
}
