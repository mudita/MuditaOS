#include <catch2/catch.hpp>

#include "Common/Query.hpp"
#include "Databases/ContactsDB.hpp"
#include "Database/Database.hpp"
#include "Databases/SmsDB.hpp"
#include "SMSRecord.hpp"
#include "ThreadRecord.hpp"
#include "queries/sms/QuerySMSSearch.hpp"

#include <memory>
#include <module-db/queries/sms/QuerySMSGetCount.hpp>
#include <module-utils/json/json11.hpp>

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
    Database::initialize();
    auto contactsDB      = std::make_unique<ContactsDB>();
    auto smsDB           = std::make_unique<SmsDB>();
    auto smsInterface    = std::make_unique<SMSRecordInterface>(smsDB.get(), contactsDB.get());
    auto threadInterface = std::make_unique<ThreadRecordInterface>(smsDB.get(), contactsDB.get());

    REQUIRE(contactsDB);
    REQUIRE(smsDB);
    REQUIRE(smsInterface);

    SECTION("unknown query -> no results")
    {
        REQUIRE(smsInterface->runQuery(std::make_shared<db::TestQuery>()) == nullptr);
    }

    auto query = std::make_shared<db::query::SMSSearch>("a", 0, 10);

    SECTION("known query, wrong interface")
    {
        auto result = smsInterface->runQuery(query);
        REQUIRE(result == nullptr);
    }

    SECTION("proper result returned")
    {
        auto result = threadInterface->runQuery(query);
        REQUIRE(dynamic_cast<db::query::SMSSearchResult *>(result.get()));
    }

    SECTION("Endpoint callback test")
    {
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
}
