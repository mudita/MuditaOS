// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <service-cellular/src/SMSSendHandler.hpp>

static SMSRecord buildValidSMSRecord()
{
    SMSRecord rec;
    rec.ID = DB_ID_NONE + 1;
    return rec;
}

TEST_CASE("SMSSendHandler functionality")
{
    using namespace cellular::internal::sms;
    SMSSendHandler outSMS;

    uint32_t wasOnSendQueryInvoked{};
    uint32_t wasOnSendInvoked{};
    outSMS.onSendQuery = [&wasOnSendQueryInvoked](db::Interface::Name database,
                                                  std::unique_ptr<db::Query> query) -> bool {
        wasOnSendQueryInvoked++;
        return true;
    };
    outSMS.onSend = [&wasOnSendInvoked](SMSRecord &record) -> bool {
        wasOnSendInvoked++;
        return true;
    };

    outSMS.onGetOfflineMode = []() -> bool { return false; };

    outSMS.onSIMNotInitialized = []() -> bool { return false; };

    SECTION("Schedule standard send SMS procedure")
    {
        auto record = buildValidSMSRecord();

        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record);
        outSMS.handleNoMoreDbRecords();

        CHECK(wasOnSendInvoked == 1);
        CHECK(wasOnSendQueryInvoked == 2);
    }

    SECTION("Schedule non-standard send SMS procedure")
    {
        auto record = buildValidSMSRecord();

        outSMS.handleDBNotification();
        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record);
        outSMS.handleDBNotification();
        outSMS.handleNoMoreDbRecords();

        CHECK(wasOnSendInvoked == 1);
        CHECK(wasOnSendQueryInvoked == 2);
    }
    SECTION("Schedule queued send SMS procedure")
    {
        auto record = buildValidSMSRecord();

        outSMS.handleDBNotification();
        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record);
        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record);
        outSMS.handleDBNotification();
        outSMS.handleDBNotification();
        outSMS.handleNoMoreDbRecords();

        CHECK(wasOnSendInvoked == 2);
        CHECK(wasOnSendQueryInvoked == 3);
    }

    SECTION("Check offline mode")
    {
        outSMS.onGetOfflineMode = []() -> bool { return true; };
        auto record             = buildValidSMSRecord();

        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record);

        CHECK(wasOnSendInvoked == 0);
        CHECK(wasOnSendQueryInvoked == 3);
    }
}
