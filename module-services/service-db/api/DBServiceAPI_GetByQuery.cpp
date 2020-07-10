#include "DBServiceAPI.hpp"

#include <includes/DBServiceName.hpp>
#include <messages/QueryMessage.hpp>
#include <Service/Message.hpp>

#include <utility>

bool DBServiceAPI::GetQuery(sys::Service *serv, db::Interface::Name database, std::unique_ptr<db::Query> query)
{
    auto msg = std::make_shared<db::QueryMessage>(database, std::move(query));
    return sys::Bus::SendUnicast(msg, service::name::db, serv);
}

sys::MessageRet_t DBServiceAPI::GetQueryWithReply(sys::Service *serv,
                                                  db::Interface::Name database,
                                                  std::unique_ptr<db::Query> query,
                                                  std::uint32_t timeout)
{
    auto msg = std::make_shared<db::QueryMessage>(database, std::move(query));
    return sys::Bus::SendUnicast(msg, service::name::db, serv, timeout);
}
