// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
        constexpr auto sendOnDelay = false;

        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record, sendOnDelay);
        outSMS.handleNoMoreDbRecords();

        CHECK(wasOnSendInvoked == 1);
        CHECK(wasOnSendQueryInvoked == 2);
    }

    SECTION("Schedule non-standard send SMS procedure")
    {
        auto record = buildValidSMSRecord();
        constexpr auto sendOnDelay = false;

        outSMS.handleDBNotification();
        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record, sendOnDelay);
        outSMS.handleDBNotification();
        outSMS.handleNoMoreDbRecords();

        CHECK(wasOnSendInvoked == 1);
        CHECK(wasOnSendQueryInvoked == 2);
    }
    SECTION("Schedule queued send SMS procedure")
    {
        auto record = buildValidSMSRecord();
        constexpr auto sendOnDelay = false;

        outSMS.handleDBNotification();
        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record, sendOnDelay);
        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record, sendOnDelay);
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
        constexpr auto sendOnDelay = false;

        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record, sendOnDelay);

        CHECK(wasOnSendInvoked == 0);
        CHECK(wasOnSendQueryInvoked == 3);
    }

    SECTION("Delayed send - positive flow")
    {
        auto record                = buildValidSMSRecord();
        constexpr auto sendOnDelay = true;

        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record, sendOnDelay);
        CHECK(wasOnSendInvoked == 0);
        CHECK(wasOnSendQueryInvoked == 1);
        outSMS.sendMessageIfDelayed();
        CHECK(wasOnSendInvoked == 1);
        CHECK(wasOnSendQueryInvoked == 2);
    }

    SECTION("Delayed send - negative flow")
    {
        auto record                = buildValidSMSRecord();
        constexpr auto sendOnDelay = false;

        outSMS.handleDBNotification();
        outSMS.handleIncomingDbRecord(record, sendOnDelay);
        CHECK(wasOnSendInvoked == 1);
        CHECK(wasOnSendQueryInvoked == 2);
        outSMS.sendMessageIfDelayed();
        CHECK(wasOnSendInvoked == 1);
        CHECK(wasOnSendQueryInvoked == 2);
    }
}
