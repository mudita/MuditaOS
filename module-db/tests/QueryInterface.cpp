#include "Common/Query.hpp"
#include "Databases/ContactsDB.hpp"
#include "Databases/SmsDB.hpp"
#include "SMSRecord.hpp"
#include "ThreadRecord.hpp"
#include "catch.hpp"
#include "../Database/Database.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include <memory>

namespace db
{

    class TestQuery : public Query
    {
      public:
        [[nodiscard]] auto debugInfo() const -> std::string override
        {
            return "Test!";
        }
    };
} // namespace db

TEST_CASE("Query interface")
{
    Database::Initialize();
    auto contactsDB      = std::make_unique<ContactsDB>();
    auto smsDB           = std::make_unique<SmsDB>();
    auto smsInterface    = std::make_unique<SMSRecordInterface>(smsDB.get(), contactsDB.get());
    auto threadInterface = std::make_unique<ThreadRecordInterface>(smsDB.get(), contactsDB.get());

    REQUIRE(contactsDB);
    REQUIRE(smsDB);
    REQUIRE(smsInterface);

    // unknown query -> no results
    REQUIRE(smsInterface->getByQuery(std::make_unique<db::TestQuery>().get()) == nullptr);

    // known query, wrong interface
    auto query  = std::make_unique<db::query::SMSSearch>("a", 0, 10);
    auto result = smsInterface->getByQuery(query.get());
    REQUIRE(result == nullptr);

    result = threadInterface->getByQuery(query.get());
    // proper result returned
    REQUIRE(dynamic_cast<db::query::SMSSearchResult *>(result.get()));
}
