// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Common/Query.hpp"
#include "Databases/ContactsDB.hpp"
#include "Database/Database.hpp"
#include "Databases/SmsDB.hpp"
#include "SMSRecord.hpp"
#include "ThreadRecord.hpp"
#include "queries/messages/threads/QueryThreadsSearchForList.hpp"
#include "queries/messages/sms/QuerySMSAdd.hpp"
#include "queries/messages/sms/QuerySMSRemove.hpp"
#include "queries/messages/sms/QuerySMSUpdate.hpp"

#include <filesystem>
#include <memory>
#include <module-db/queries/messages/sms/QuerySMSGetCount.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <json11.hpp>

TEST_CASE("Multi Query interface")
{
    Database::initialize();

    auto contactsDB      = std::make_unique<ContactsDB>((std::filesystem::path{"sys/user"} / "contacts.db").c_str());
    auto smsDB           = std::make_unique<SmsDB>((std::filesystem::path{"sys/user"} / "sms.db").c_str());
    auto smsInterface    = std::make_unique<SMSRecordInterface>(smsDB.get(), contactsDB.get());
    auto threadInterface = std::make_unique<ThreadRecordInterface>(smsDB.get(), contactsDB.get());

    REQUIRE(contactsDB);
    REQUIRE(smsDB);
    REQUIRE(smsInterface);

    SECTION("")
    {}
}
