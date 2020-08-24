#include <catch2/catch.hpp>

#include <Database/Database.hpp>
#include <Databases/ContactsDB.hpp>
#include <Databases/SmsDB.hpp>
#include <Interface/ContactRecord.hpp>
#include <Interface/SMSRecord.hpp>
#include <Interface/ThreadRecord.hpp>

#include <country.hpp>
#include <PhoneNumber.hpp>
#include <vfs.hpp>

#include <algorithm>

#include <cstdint>
#include <cstdio>
#include <cstring>

struct test
{
    UTF8 test;
    UTF8 test2;
};

TEST_CASE("SMS Record tests")
{
    Database::initialize();

    vfs.remove(ContactsDB::GetDBName());
    vfs.remove(SmsDB::GetDBName());

    auto smsDB      = std::make_unique<SmsDB>();
    auto contactsDB = std::make_unique<ContactsDB>();

    const uint32_t dateTest      = 123456789;
    const uint32_t dateSentTest  = 987654321;
    const uint32_t errorCodeTest = 555;
    auto numberTest              = utils::PhoneNumber("+48600123456", utils::country::Id::UNKNOWN).getView();
    auto numberTest2             = utils::PhoneNumber("222333444", utils::country::Id::UNKNOWN).getView();
    const char *bodyTest         = "Test SMS Body";
    const char *bodyTest2        = "Test SMS Body2";
    const SMSType typeTest       = SMSType ::DRAFT;

    SMSRecordInterface smsRecInterface(smsDB.get(), contactsDB.get());

    SMSRecord recordIN;
    recordIN.date      = dateTest;
    recordIN.dateSent  = dateSentTest;
    recordIN.errorCode = errorCodeTest;
    recordIN.number    = numberTest;
    recordIN.body      = bodyTest;
    recordIN.type      = typeTest;

    // Add 2 records
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.Add(recordIN));

    // Get all available records
    auto records = smsRecInterface.GetLimitOffset(0, 100);
    REQUIRE(records->size() == 2);

    // Check if fetched records contain valid data
    for (const auto &w : *records) {
        REQUIRE(w.body == bodyTest);
        REQUIRE(w.number == numberTest);
    }

    // Get all available records by specified thread ID and check for invalid data
    records = smsRecInterface.GetLimitOffsetByField(0, 100, SMSRecordField::ThreadID, "1");
    REQUIRE((*records).size() == 2);
    for (const auto &w : *records) {
        REQUIRE(w.body == bodyTest);
        REQUIRE(w.number == numberTest);
    }

    // Get all available records by specified contact ID and check for invalid data
    records = smsRecInterface.GetLimitOffsetByField(0, 100, SMSRecordField::ContactID, "1");
    REQUIRE((*records).size() == 2);
    for (const auto &w : *records) {
        REQUIRE(w.body == bodyTest);
        REQUIRE(w.number == numberTest);
    }

    // Remove records one by one
    REQUIRE(smsRecInterface.RemoveByID(1));
    REQUIRE(smsRecInterface.RemoveByID(2));

    // SMS database should not contain any records
    REQUIRE(smsRecInterface.GetCount() == 0);

    // Test updating record
    REQUIRE(smsRecInterface.Add(recordIN));
    recordIN.ID   = 1;
    recordIN.body = bodyTest2;
    REQUIRE(smsRecInterface.Update(recordIN));

    auto record = smsRecInterface.GetByID(1);
    REQUIRE(record.ID != 0);
    REQUIRE(record.body == bodyTest2);

    // SMS database should contain 1 record
    REQUIRE(smsRecInterface.GetCount() == 1);

    // Remove existing record
    REQUIRE(smsRecInterface.RemoveByID(1));
    // SMS database should be empty
    REQUIRE(smsRecInterface.GetCount() == 0);

    // Test fetching record by using Thread ID
    // Add records with different numbers, they should be placed in separate threads&contacts dbs
    recordIN.body   = bodyTest;
    recordIN.number = numberTest;
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.Add(recordIN));

    recordIN.number = numberTest2;
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.Add(recordIN));

    // Get all available records by specified thread ID and check for invalid data
    records = smsRecInterface.GetLimitOffsetByField(0, 100, SMSRecordField::ThreadID, "1");
    REQUIRE((*records).size() == 2);
    for (const auto &w : *records) {
        REQUIRE(w.body == bodyTest);
        REQUIRE(w.number == numberTest);
    }

    // Get all available records by specified thread ID and check for invalid data
    records = smsRecInterface.GetLimitOffsetByField(0, 100, SMSRecordField::ThreadID, "2");
    REQUIRE((*records).size() == 2);
    for (const auto &w : *records) {
        REQUIRE(w.body == bodyTest);
        REQUIRE(w.number == numberTest2);
    }

    // Get all available records by specified contact ID and check for invalid data
    records = smsRecInterface.GetLimitOffsetByField(0, 100, SMSRecordField::ContactID, "1");
    REQUIRE((*records).size() == 2);
    for (const auto &w : *records) {
        REQUIRE(w.body == bodyTest);
        REQUIRE(w.number == numberTest);
    }

    // Get all available records by specified contact ID and check for invalid data
    records = smsRecInterface.GetLimitOffsetByField(0, 100, SMSRecordField::ContactID, "2");
    REQUIRE((*records).size() == 2);
    for (const auto &w : *records) {
        REQUIRE(w.body == bodyTest);
        REQUIRE(w.number == numberTest2);
    }

    // Remove sms records in order to check automatic management of threads and contact databases
    ThreadRecordInterface threadRecordInterface(smsDB.get(), contactsDB.get());
    REQUIRE(smsRecInterface.RemoveByID(1));
    records = smsRecInterface.GetLimitOffsetByField(0, 100, SMSRecordField::ContactID, "1");
    REQUIRE((*records).size() == 1);

    REQUIRE(threadRecordInterface.GetCount() == 2);

    REQUIRE(smsRecInterface.RemoveByID(2));
    records = smsRecInterface.GetLimitOffsetByField(0, 100, SMSRecordField::ContactID, "1");
    REQUIRE((*records).size() == 0);
    REQUIRE(threadRecordInterface.GetCount() == 1);

    REQUIRE(smsRecInterface.RemoveByID(3));
    REQUIRE(smsRecInterface.RemoveByID(4));
    REQUIRE(threadRecordInterface.GetCount() == 0);

    // Test removing a message which belongs to non-existent thread
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsDB->threads.removeById(1)); // stealthy thread remove
    REQUIRE(smsRecInterface.RemoveByID(1));

    // Test handling of missmatch in sms vs. thread tables
    auto trueCount = 10;
    // prepare
    for (auto added = 0; added < trueCount; added++) {
        recordIN.date = added;                     // for proper order
        recordIN.body = std::to_string(added + 1); // == ID
        REQUIRE(smsRecInterface.Add(recordIN));    // threadID = 1
    }
    ThreadRecord threadRec = threadRecordInterface.GetByID(1);
    REQUIRE(threadRec.isValid());
    ThreadsTableRow threadRaw{{.ID = threadRec.ID},
                              .date           = threadRec.date,
                              .msgCount       = threadRec.msgCount,
                              .unreadMsgCount = threadRec.unreadMsgCount,
                              .contactID      = threadRec.contactID,
                              .snippet        = threadRec.snippet,
                              .type           = threadRec.type};
    threadRaw.msgCount = trueCount + 1; // break the DB
    REQUIRE(smsDB->threads.update(threadRaw));

    REQUIRE(static_cast<int>(smsRecInterface.GetLimitOffsetByField(0, 100, SMSRecordField::ThreadID, "1")->size()) ==
            trueCount);
    // end of preparation, now test
    for (auto latest = trueCount; latest > 0; latest--) {
        REQUIRE(smsRecInterface.RemoveByID(latest)); // remove the latest
        switch (latest) {                            // was just removed
        case 3:                                      // remaining 2 or more
        default:
            REQUIRE(threadRecordInterface.GetByID(1).snippet.c_str() == std::to_string(latest - 1)); // next to newest
            break;
        case 2:                                                                             // remaining 1
            REQUIRE(threadRecordInterface.GetByID(1).snippet.c_str() == std::to_string(1)); // only one remaining
            break;
        case 1: // no sms remaining
            // make sure there is no thread nor sms
            REQUIRE(threadRecordInterface.GetCount() == 0);
            REQUIRE(smsRecInterface.GetCount() == 0);
            break;
        }
    }

    // Test removing by field
    recordIN.number = numberTest;
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.RemoveByField(SMSRecordField::ThreadID, "1"));
    REQUIRE(smsRecInterface.GetCount() == 0);

    recordIN.number = numberTest;
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.Add(recordIN));
    REQUIRE(smsRecInterface.RemoveByField(SMSRecordField::ContactID, "1"));
    REQUIRE(smsRecInterface.GetCount() == 0);

    Database::deinitialize();
}
