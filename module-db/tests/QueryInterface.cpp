// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Helpers.hpp"
#include "Common/Query.hpp"
#include "module-db/databases/ContactsDB.hpp"
#include "Database/Database.hpp"
#include "module-db/databases/SmsDB.hpp"
#include "SMSRecord.hpp"
#include "ThreadRecord.hpp"
#include "queries/messages/threads/QueryThreadsSearchForList.hpp"
#include "queries/messages/sms/QuerySMSAdd.hpp"
#include "queries/messages/sms/QuerySMSRemove.hpp"
#include "queries/messages/sms/QuerySMSUpdate.hpp"

#include <filesystem>
#include <memory>
#include <module-db/queries/messages/sms/QuerySMSGetCount.hpp>
#include <json11.hpp>

namespace db
{

    class TestQuery : public Query
    {
      public:
        TestQuery() : Query(Query::Type::Read)
        {}

        [[nodiscard]] auto debugInfo() const -> std::string override
        {
            return "Test!";
        }
    };
} // namespace db

TEST_CASE("Query interface")
{
    db::tests::DatabaseUnderTest<ContactsDB> contactsDb{"contacts.db", db::tests::getPurePhoneScriptsPath()};
    db::tests::DatabaseUnderTest<SmsDB> smsDb{"sms.db", db::tests::getPurePhoneScriptsPath()};

    auto smsInterface    = std::make_unique<SMSRecordInterface>(&smsDb.get(), &contactsDb.get());
    auto threadInterface = std::make_unique<ThreadRecordInterface>(&smsDb.get(), &contactsDb.get());

    REQUIRE(smsInterface);

    SECTION("unknown query -> no results")
    {
        REQUIRE(smsInterface->runQuery(std::make_shared<db::TestQuery>()) == nullptr);
    }

    auto query = std::make_shared<db::query::ThreadsSearchForList>("a", 0, 10);

    SECTION("known query, wrong interface")
    {
        auto result = smsInterface->runQuery(query);
        REQUIRE(result == nullptr);
    }

    SECTION("proper result returned")
    {
        auto result = threadInterface->runQuery(query);
        REQUIRE(dynamic_cast<db::query::ThreadsSearchResultForList *>(result.get()));
    }

    SECTION("Endpoint callback test")
    {
        using sdesktop::endpoints::Context;

        std::shared_ptr<db::Query> query = std::make_shared<db::query::SMSGetCount>();
        auto testMessage                 = R"({"endpoint":6, "method":1, "uuid":12345, "body":{"test":"test"}})";
        std::string err;
        auto msgJson = json11::Json::parse(testMessage, err);
        REQUIRE(err.empty());

        Context context(msgJson);
        auto listener = std::make_unique<db::EndpointListener>(
            [=](db::QueryResult *result, Context &context) {
                if (auto SMSResult = dynamic_cast<db::query::SMSGetCountResult *>(result)) {
                    auto id   = SMSResult->getResults();
                    auto body = json11::Json::object{{"count", static_cast<int>(id)}};
                    return true;
                }
                else {
                    return false;
                }
            },
            context);

        query->setQueryListener(std::move(listener));
        auto result        = smsInterface->runQuery(query);
        auto queryListener = result->getRequestQuery()->getQueryListener();
        REQUIRE(queryListener->handleQueryResponse(result.get()) == true);
    }

    SECTION("SMS create -> update -> remove")
    {
        constexpr char smsBody[] = "text";

        SMSRecord record{};
        record.body = smsBody;

        using db::query::SMSAdd;
        using db::query::SMSAddResult;

        auto addQuery          = std::make_shared<SMSAdd>(record);
        const auto response    = smsInterface->runQuery(addQuery);
        const auto addResponse = static_cast<SMSAddResult *>(response.get());
        REQUIRE(addResponse->result == true);
        REQUIRE(addResponse->record.ID != DB_ID_NONE);

        record = addResponse->record;

        SECTION("Update SMS entry")
        {
            using db::query::SMSUpdate;
            using db::query::SMSUpdateResult;
            constexpr char updatedSmsBody[] = "updated_text";

            record.body               = updatedSmsBody;
            auto updateQuery          = std::make_shared<SMSUpdate>(record);
            const auto updateResponse = smsInterface->runQuery(updateQuery);
            REQUIRE(static_cast<SMSUpdateResult *>(updateResponse.get())->result == true);
        }

        SECTION("Remove SMS entry")
        {
            using db::query::SMSRemove;
            using db::query::SMSRemoveResult;

            auto removeQuery          = std::make_shared<SMSRemove>(record.ID);
            const auto removeResponse = smsInterface->runQuery(removeQuery);
            REQUIRE(static_cast<SMSRemoveResult *>(removeResponse.get())->getResults() == true);
        }
    }
}
