// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Endpoint.hpp>
#include <endpoints/EndpointFactory.hpp>

#include <catch2/catch.hpp>
#include <json11.hpp>
#include <purefs/filesystem_paths.hpp>
#include <utf8/UTF8.hpp>

#include <memory>
#include <filesystem>
#include <string>
#include <vector>

#include <module-db/Interface/ContactRecord.hpp>
#include <module-db/Databases/ContactsDB.hpp>

std::unique_ptr<ContactsDB> contactsDb = nullptr;
std::vector<std::string> messageStrings;

// stubs
std::pair<bool, std::uint64_t> DBServiceAPI::GetQuery(sys::Service *serv,
                                                      db::Interface::Name database,
                                                      std::unique_ptr<db::Query> query)
{
    auto queryType = query->type;
    LOG_DEBUG("query is: %d", static_cast<uint32_t>(queryType));

    ContactRecordInterface cri(contactsDb.get());
    auto result        = cri.runQuery(std::move(query));
    auto queryListener = result->getRequestQuery()->getQueryListener();
    queryListener->handleQueryResponse(result.get());

    return std::make_pair(true, 0);
}

auto DBServiceAPI::GetQueryWithReply(sys::Service *serv,
                                     db::Interface::Name database,
                                     std::unique_ptr<db::Query> query,
                                     std::uint32_t timeout) -> sys::SendResult
{
    return {};
}

auto DBServiceAPI::verifyContact(sys::Service *serv, const ContactRecord &rec) -> ContactVerificationResult
{
    return {};
}
auto DBServiceAPI::ContactGetByID(sys::Service *serv, uint32_t contactID) -> std::unique_ptr<std::vector<ContactRecord>>
{
    return nullptr;
}
auto DBServiceAPI::ContactGetByIDWithTemporary(sys::Service *serv, uint32_t contactID)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    return nullptr;
}
auto DBServiceAPI::ContactGetByIDCommon(sys::Service *serv, std::shared_ptr<DBContactMessage> contactMsg)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    return nullptr;
}
auto DBServiceAPI::ContactGetBySpeeddial(sys::Service *serv, UTF8 speeddial)
    -> std::unique_ptr<std::vector<ContactRecord>>
{
    return nullptr;
}
auto DBServiceAPI::MatchContactByPhoneNumber(sys::Service *serv, const utils::PhoneNumber::View &numberView)
    -> std::unique_ptr<ContactRecord>
{
    return nullptr;
}
auto DBServiceAPI::ContactAdd(sys::Service *serv, const ContactRecord &rec) -> bool
{
    return false;
}
auto DBServiceAPI::ContactRemove(sys::Service *serv, uint32_t id) -> bool
{
    return false;
}
auto DBServiceAPI::ContactUpdate(sys::Service *serv, const ContactRecord &rec) -> bool
{
    return false;
}
auto DBServiceAPI::CalllogAdd(sys::Service *serv, const CalllogRecord &rec) -> CalllogRecord
{
    return {};
}
auto DBServiceAPI::CalllogRemove(sys::Service *serv, uint32_t id) -> bool
{
    return false;
}
auto DBServiceAPI::CalllogUpdate(sys::Service *serv, const CalllogRecord &rec) -> bool
{
    return false;
}
auto DBServiceAPI::DBBackup(sys::Service *serv, std::string backupPath) -> bool
{
    return false;
}
bool DBServiceAPI::AddSMS(sys::Service *serv, const SMSRecord &record, std::unique_ptr<db::QueryListener> &&listener)
{
    return false;
}

xQueueHandle parserFSM::MessageHandler::sendQueue;

parserFSM::MessageHandler::MessageHandler(sys::Service *OwnerService, std::unique_ptr<EndpointFactory> endpointFactory)
    : OwnerServicePtr(OwnerService), endpointFactory(std::move(endpointFactory))
{}

void parserFSM::MessageHandler::parseMessage(const std::string &msg)
{}

void parserFSM::MessageHandler::processMessage()
{}

void parserFSM::MessageHandler::putToSendQueue(const std::string &msg)
{
    messageStrings.push_back(msg);
    LOG_DEBUG("response is: %s", msg.c_str());
}
//~stubs

TEST_CASE("Endpoint Contacts Test")
{
    Database::initialize();

    const auto contactsPath = purefs::dir::getUserDiskPath() / "contacts.db";
    std::filesystem::remove(contactsPath);

    contactsDb = std::make_unique<ContactsDB>(contactsPath.c_str());
    REQUIRE(contactsDb->isInitialized());
    messageStrings.clear();

    SECTION("Request with pages")
    {
        auto count       = 29; // requested number of record to return
        auto endpoint    = 7;
        auto uuid        = 1103;
        auto totalCount  = contactsDb->contacts.count();
        auto testMessage = "{\"endpoint\":" + std::to_string(endpoint) +
                           ", \"method\":1, \"uuid\":" + std::to_string(uuid) +
                           ", \"body\":{\"limit\":" + std::to_string(count) + ", \"offset\":20}}";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        parserFSM::PagedContext context(msgJson, 10);
        auto factory = std::make_unique<SecuredEndpointFactory>(EndpointSecurity::Allow);
        auto handler = factory->create(context, nullptr);
        handler->handle(context);
        auto pageSize = context.getPageSize();
        REQUIRE(10 == pageSize);
        REQUIRE(1 == messageStrings.size());
        auto msg = messageStrings[0];
        REQUIRE(msg.size() > 10);
        auto retJson = json11::Json::parse(msg.substr(10), err); // string length and go to real data

        REQUIRE(err.empty());
        REQUIRE(uuid == retJson[parserFSM::json::uuid].int_value());
        REQUIRE(endpoint == retJson[parserFSM::json::endpoint].int_value());

        auto body = retJson[parserFSM::json::body];
        REQUIRE(totalCount == static_cast<uint32_t>(body[parserFSM::json::totalCount].int_value()));

        auto nextPage = body[parserFSM::json::nextPage];
        REQUIRE(static_cast<int>(pageSize) == nextPage[parserFSM::json::limit].int_value());
        // base offset + pageSize cause asked for 29
        REQUIRE(30 == nextPage[parserFSM::json::offset].int_value());

        auto entries = body[parserFSM::json::entries];
        REQUIRE(pageSize == entries.array_items().size());
    }
}
