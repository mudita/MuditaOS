// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/EventsDB.hpp"
#include "Tables/EventsTable.hpp"

#include <vfs.hpp>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <purefs/filesystem_paths.hpp>
#include <unistd.h>

static auto remove_events(EventsDB &db) -> bool
{
    auto count   = db.events.count();
    auto records = db.events.getLimitOffset(0, count);
    bool ret     = true;
    for (auto rec : records) {
        ret = ret && db.events.removeById(rec.ID);
    }
    return ret;
}

TEST_CASE("Events Table tests")
{

    Database::initialize();
    vfs.Init();

    const auto eventsPath = purefs::dir::getUserDiskPath() / "events.db";
    std::filesystem::remove(eventsPath);

    EventsDB eventsDb{eventsPath.c_str()};
    REQUIRE(eventsDb.isInitialized());

    auto &eventsTbl = eventsDb.events;

    if (eventsTbl.count() > 0) {
        REQUIRE(remove_events(eventsDb));
    }

    REQUIRE(eventsTbl.count() == 0);

    SECTION("Default Constructor")
    {
        EventsTableRow testRow;
        CHECK(testRow.ID == DB_ID_NONE);
        CHECK(testRow.UID == "");
        CHECK(testRow.title == "");
        CHECK(testRow.date_from == TIME_POINT_INVALID);
        CHECK(testRow.date_till == TIME_POINT_INVALID);
        CHECK(testRow.reminder == 0);
        CHECK(testRow.repeat == 0);
        CHECK(testRow.reminder_fired == TIME_POINT_INVALID);
        CHECK(testRow.provider_id == "");
        CHECK_FALSE(testRow.isValid());
    }

    EventsTableRow testRow1 = {{1},
                               .UID              = "test1",
                               .title            = "Event1",
                               .date_from        = TimePointFromString("2019-10-20 14:25:00"),
                               .date_till        = TimePointFromString("2019-10-20 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TimePointFromString("2019-10-20 14:20:00"),
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    EventsTableRow testRow2 = {{2},
                               .UID              = "test2",
                               .title            = "Event2",
                               .date_from        = TimePointFromString("2019-10-21 14:24:00"),
                               .date_till        = TimePointFromString("2019-10-21 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TimePointFromString("2019-10-21 14:20:00"),
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    EventsTableRow testRow3 = {{3},
                               .UID              = "test3",
                               .title            = "Event3",
                               .date_from        = TimePointFromString("2019-10-22 14:25:00"),
                               .date_till        = TimePointFromString("2019-10-22 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TimePointFromString("2019-10-22 14:20:00"),
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    EventsTableRow testRow4 = {{4},
                               .UID              = "test4",
                               .title            = "Event4",
                               .date_from        = TimePointFromString("2019-10-23 14:25:00"),
                               .date_till        = TimePointFromString("2019-10-23 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TimePointFromString("2019-10-23 14:20:00"),
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    EventsTableRow testRow5 = {{5},
                               .UID              = "test5",
                               .title            = "Event5",
                               .date_from        = TimePointFromString("2019-10-24 14:25:00"),
                               .date_till        = TimePointFromString("2019-10-24 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TimePointFromString("2019-10-24 14:20:00"),
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    EventsTableRow testRow6 = {{6},
                               .UID              = "test6",
                               .title            = "Event6",
                               .date_from        = TimePointFromString("2019-10-24 14:25:00"),
                               .date_till        = TimePointFromString("2019-10-24 15:36:00"),
                               .reminder         = static_cast<uint32_t>(Reminder::five_min_before),
                               .repeat           = static_cast<uint32_t>(Repeat::never),
                               .reminder_fired   = TimePointFromString("2019-10-24 14:20:00"),
                               .provider_type    = "PurePhone",
                               .provider_id      = "testID",
                               .provider_iCalUid = "test6"};

    /// add
    CHECK(eventsTbl.add(testRow1));
    CHECK(eventsTbl.add(testRow2));
    CHECK(eventsTbl.add(testRow3));
    CHECK(eventsTbl.add(testRow4));
    CHECK(eventsTbl.add(testRow5));
    CHECK(eventsTbl.add(testRow6));
    CHECK(eventsTbl.count() == 6);

    auto check_tableRow = [](const EventsTableRow &actual, const EventsTableRow &expected) {
        CHECK(actual.ID == expected.ID);
        CHECK(actual.UID == expected.UID);
        CHECK(actual.title == expected.title);
        CHECK(TimePointToString(actual.date_from) == TimePointToString(expected.date_from));
        CHECK(TimePointToString(actual.date_till) == TimePointToString(expected.date_till));
        CHECK(actual.reminder == expected.reminder);
        CHECK(actual.repeat == expected.repeat);
        CHECK(actual.reminder_fired == expected.reminder_fired);
        CHECK(actual.provider_type == expected.provider_type);
        CHECK(actual.provider_id == expected.provider_id);
        CHECK(actual.provider_iCalUid == expected.provider_iCalUid);
        CHECK(actual.isValid());
    };

    SECTION("Check add daily")
    {
        if (eventsTbl.count() > 0) {
            REQUIRE(remove_events(eventsDb));
        }
        CHECK(eventsTbl.count() == 0);

        uint32_t numberOfEvents = 7;
        TimePoint startDate     = TimePointFromString("2019-10-20 14:30:00");
        TimePoint endDate       = TimePointFromString("2019-10-20 15:30:00");
        testRow1.date_from      = startDate;
        testRow1.date_till      = endDate;
        CHECK(eventsTbl.addDaily(testRow1));
        CHECK(eventsTbl.count() == numberOfEvents);

        auto entries = eventsTbl.selectByDatePeriod(
            TimePointFromString("2019-10-19 10:00:00"), TimePointFromString("2020-10-30 10:00:00"), 0, UINT32_MAX);
        CHECK(entries.size() == numberOfEvents);

        for (auto entry : entries) {
            CHECK(entry.title == testRow1.title);
            CHECK(TimePointToString(entry.date_from) == TimePointToString(startDate));
            CHECK(TimePointToString(entry.date_till) == TimePointToString(endDate));
            CHECK(entry.reminder == testRow1.reminder);
            CHECK(entry.repeat == testRow1.repeat);
            CHECK(entry.reminder_fired == testRow1.reminder_fired);
            CHECK(entry.provider_type == testRow1.provider_type);
            CHECK(entry.provider_id == testRow1.provider_id);
            CHECK(entry.provider_iCalUid == testRow1.provider_iCalUid);
            CHECK(entry.isValid());
            startDate += 24h;
            endDate += 24h;
        }
    }

    SECTION("Check add weekly")
    {
        if (eventsTbl.count() > 0) {
            REQUIRE(remove_events(eventsDb));
        }
        CHECK(eventsTbl.count() == 0);

        uint32_t numberOfEvents = 4;
        TimePoint startDate     = TimePointFromString("2019-10-20 14:30:00");
        TimePoint endDate       = TimePointFromString("2019-10-20 15:30:00");
        testRow1.date_from      = startDate;
        testRow1.date_till      = endDate;
        CHECK(eventsTbl.addWeekly(testRow1));
        CHECK(eventsTbl.count() == numberOfEvents);

        auto entries = eventsTbl.selectByDatePeriod(
            TimePointFromString("2019-10-19 10:00:00"), TimePointFromString("2020-10-30 10:00:00"), 0, UINT32_MAX);
        CHECK(entries.size() == numberOfEvents);

        for (auto entry : entries) {
            CHECK(entry.title == testRow1.title);
            CHECK(TimePointToString(entry.date_from) == TimePointToString(startDate));
            CHECK(TimePointToString(entry.date_till) == TimePointToString(endDate));
            CHECK(entry.reminder == testRow1.reminder);
            CHECK(entry.repeat == testRow1.repeat);
            CHECK(entry.reminder_fired == testRow1.reminder_fired);
            CHECK(entry.provider_type == testRow1.provider_type);
            CHECK(entry.provider_id == testRow1.provider_id);
            CHECK(entry.provider_iCalUid == testRow1.provider_iCalUid);
            CHECK(entry.isValid());
            startDate += (24h * 7);
            endDate += (24h * 7);
        }
    }

    SECTION("Check add biweekly")
    {
        if (eventsTbl.count() > 0) {
            REQUIRE(remove_events(eventsDb));
        }
        CHECK(eventsTbl.count() == 0);

        uint32_t numberOfEvents = 4;
        TimePoint startDate     = TimePointFromString("2019-10-20 14:30:00");
        TimePoint endDate       = TimePointFromString("2019-10-20 15:30:00");
        testRow1.date_from      = startDate;
        testRow1.date_till      = endDate;
        CHECK(eventsTbl.addTwoWeeks(testRow1));
        CHECK(eventsTbl.count() == numberOfEvents);

        auto entries = eventsTbl.selectByDatePeriod(
            TimePointFromString("2019-10-19 10:00:00"), TimePointFromString("2020-10-30 10:00:00"), 0, UINT32_MAX);
        CHECK(entries.size() == numberOfEvents);

        for (auto entry : entries) {
            CHECK(entry.title == testRow1.title);
            CHECK(TimePointToString(entry.date_from) == TimePointToString(startDate));
            CHECK(TimePointToString(entry.date_till) == TimePointToString(endDate));
            CHECK(entry.reminder == testRow1.reminder);
            CHECK(entry.repeat == testRow1.repeat);
            CHECK(entry.reminder_fired == testRow1.reminder_fired);
            CHECK(entry.provider_type == testRow1.provider_type);
            CHECK(entry.provider_id == testRow1.provider_id);
            CHECK(entry.provider_iCalUid == testRow1.provider_iCalUid);
            CHECK(entry.isValid());
            startDate += (24h * 7 * 2);
            endDate += (24h * 7 * 2);
        }
    }

    SECTION("Check add monthly")
    {
        if (eventsTbl.count() > 0) {
            REQUIRE(remove_events(eventsDb));
        }
        CHECK(eventsTbl.count() == 0);

        uint32_t numberOfEvents = 12;
        const std::array<TimePoint, 24> dates{
            TimePointFromString("2019-01-20 14:30:00"), TimePointFromString("2019-01-20 15:30:00"),
            TimePointFromString("2019-02-20 14:30:00"), TimePointFromString("2019-02-20 15:30:00"),
            TimePointFromString("2019-03-20 14:30:00"), TimePointFromString("2019-03-20 15:30:00"),
            TimePointFromString("2019-04-20 14:30:00"), TimePointFromString("2019-04-20 15:30:00"),
            TimePointFromString("2019-05-20 14:30:00"), TimePointFromString("2019-05-20 15:30:00"),
            TimePointFromString("2019-06-20 14:30:00"), TimePointFromString("2019-06-20 15:30:00"),
            TimePointFromString("2019-07-20 14:30:00"), TimePointFromString("2019-07-20 15:30:00"),
            TimePointFromString("2019-08-20 14:30:00"), TimePointFromString("2019-08-20 15:30:00"),
            TimePointFromString("2019-09-20 14:30:00"), TimePointFromString("2019-09-20 15:30:00"),
            TimePointFromString("2019-10-20 14:30:00"), TimePointFromString("2019-10-20 15:30:00"),
            TimePointFromString("2019-11-20 14:30:00"), TimePointFromString("2019-11-20 15:30:00"),
            TimePointFromString("2019-12-20 14:30:00"), TimePointFromString("2019-12-20 15:30:00")};

        testRow1.date_from = dates[0];
        testRow1.date_till = dates[1];
        CHECK(eventsTbl.addMonth(testRow1));
        CHECK(eventsTbl.count() == numberOfEvents);

        auto entries = eventsTbl.selectByDatePeriod(
            TimePointFromString("1970-10-19 10:00:00"), TimePointFromString("2040-10-30 10:00:00"), 0, UINT32_MAX);
        CHECK(entries.size() == numberOfEvents);

        uint32_t index = 0;
        for (auto entry : entries) {
            CHECK(entry.title == testRow1.title);
            CHECK(TimePointToString(entry.date_from) == TimePointToString(dates[index]));
            CHECK(TimePointToString(entry.date_till) == TimePointToString(dates[index + 1]));
            CHECK(entry.reminder == testRow1.reminder);
            CHECK(entry.repeat == testRow1.repeat);
            CHECK(entry.reminder_fired == testRow1.reminder_fired);
            CHECK(entry.provider_type == testRow1.provider_type);
            CHECK(entry.provider_id == testRow1.provider_id);
            CHECK(entry.provider_iCalUid == testRow1.provider_iCalUid);
            CHECK(entry.isValid());
            index += 2;
        }
    }

    SECTION("Check add yearly")
    {
        if (eventsTbl.count() > 0) {
            REQUIRE(remove_events(eventsDb));
        }
        REQUIRE(eventsTbl.count() == 0);

        uint32_t numberOfEvents = 4;
        std::array<TimePoint, 8> dates{TimePointFromString("2019-02-20 14:30:00"),
                                       TimePointFromString("2019-02-20 15:30:00"),
                                       TimePointFromString("2020-02-20 14:30:00"),
                                       TimePointFromString("2020-02-20 15:30:00"),
                                       TimePointFromString("2021-02-20 14:30:00"),
                                       TimePointFromString("2021-02-20 15:30:00"),
                                       TimePointFromString("2022-02-20 14:30:00"),
                                       TimePointFromString("2022-02-20 15:30:00")};

        testRow1.date_from = dates[0];
        testRow1.date_till = dates[1];
        CHECK(eventsTbl.addYear(testRow1));
        CHECK(eventsTbl.count() == numberOfEvents);

        auto entries = eventsTbl.selectByDatePeriod(
            TimePointFromString("1970-10-19 10:00:00"), TimePointFromString("2045-10-30 10:00:00"), 0, UINT32_MAX);
        CHECK(entries.size() == numberOfEvents);

        uint32_t index = 0;
        for (auto entry : entries) {
            CHECK(entry.title == testRow1.title);
            CHECK(TimePointToString(entry.date_from) == TimePointToString(dates[index]));
            CHECK(TimePointToString(entry.date_till) == TimePointToString(dates[index + 1]));
            CHECK(entry.reminder == testRow1.reminder);
            CHECK(entry.repeat == testRow1.repeat);
            CHECK(entry.reminder_fired == testRow1.reminder_fired);
            CHECK(entry.provider_type == testRow1.provider_type);
            CHECK(entry.provider_id == testRow1.provider_id);
            CHECK(entry.provider_iCalUid == testRow1.provider_iCalUid);
            CHECK(entry.isValid());
            index += 2;
        }
    }

    enum class weekDayOption
    {
        monday    = 65536,
        tuesday   = 131072,
        wednesday = 262144,
        thursday  = 524288,
        friday    = 1048576,
        saturday  = 2097152,
        sunday    = 4194304,
    };

    SECTION("Check add custom")
    {
        auto check_custom_repeat = [&](uint32_t customRepeatOption,
                                       uint32_t numberOfEvents,
                                       TimePoint originalStartDate,
                                       TimePoint originalEndDate) {
            if (eventsTbl.count() > 0) {
                REQUIRE(remove_events(eventsDb));
            }
            CHECK(eventsTbl.count() == 0);

            const uint32_t numberOfWeeks = 4;
            std::vector<bool> weekDayOptions;
            weekDayOptions = eventsTbl.parseOptions(customRepeatOption);

            testRow1.repeat    = customRepeatOption;
            testRow1.date_from = originalStartDate;
            testRow1.date_till = originalEndDate;
            CHECK(eventsTbl.addCustom(testRow1));
            CHECK(eventsTbl.count() == numberOfEvents);

            auto entries = eventsTbl.selectByDatePeriod(TimePointFromString("2018-12-10 14:30:00"),
                                                        TimePointFromString("2029-12-10 14:30:00"),
                                                        0,
                                                        numberOfEvents);
            CHECK(entries.size() == numberOfEvents);
            uint32_t orginalEventID = UINT32_MAX;
            for (auto entry : entries) {
                if (orginalEventID > entry.ID) {
                    orginalEventID = entry.ID;
                }
            }

            TimePoint expectedStartDate = TimePointFromString("2020-12-07 14:30:00"); // monday
            TimePoint expectedEndDate   = TimePointFromString("2020-12-07 15:30:00"); // monday

            uint32_t i = 0;
            for (uint32_t l = 0; l < numberOfWeeks; l++) {
                for (uint32_t j = 0; j < weekDayOptions.size(); j++) {
                    if (entries[i].ID == orginalEventID) {
                        CHECK(entries[i].title == testRow1.title);
                        CHECK(TimePointToString(entries[i].date_from) == TimePointToString(originalStartDate));
                        CHECK(TimePointToString(entries[i].date_till) == TimePointToString(originalEndDate));
                        CHECK(entries[i].reminder == testRow1.reminder);
                        CHECK(entries[i].repeat == testRow1.repeat);
                        CHECK(entries[i].reminder_fired == testRow1.reminder_fired);
                        CHECK(entries[i].provider_type == testRow1.provider_type);
                        CHECK(entries[i].provider_id == testRow1.provider_id);
                        CHECK(entries[i].provider_iCalUid == testRow1.provider_iCalUid);
                        CHECK(entries[i].isValid());
                        i++;
                    }
                    else if (weekDayOptions[j]) {
                        CHECK(entries[i].title == testRow1.title);
                        CHECK(TimePointToString(entries[i].date_from) == TimePointToString(expectedStartDate));
                        CHECK(TimePointToString(entries[i].date_till) == TimePointToString(expectedEndDate));
                        CHECK(entries[i].reminder == testRow1.reminder);
                        CHECK(entries[i].repeat == testRow1.repeat);
                        CHECK(entries[i].reminder_fired == testRow1.reminder_fired);
                        CHECK(entries[i].provider_type == testRow1.provider_type);
                        CHECK(entries[i].provider_id == testRow1.provider_id);
                        CHECK(entries[i].provider_iCalUid == testRow1.provider_iCalUid);
                        CHECK(entries[i].isValid());
                        i++;
                    }
                    expectedStartDate += 24h;
                    expectedEndDate += 24h;
                }
            }
        };

        SECTION("Repeat Mondays and Wednsdays (original thursday)")
        {
            uint32_t customRepeatOption =
                static_cast<uint32_t>(weekDayOption::monday) + static_cast<uint32_t>(weekDayOption::wednesday);
            uint32_t numberOfEvents     = 9;
            TimePoint originalStartDate = TimePointFromString("2020-12-10 14:30:00"); // thursday
            TimePoint originalEndDate   = TimePointFromString("2020-12-10 15:30:00"); // thursday
            LOG_DEBUG("start: %s", TimePointToString(originalStartDate + date::days{0}).c_str());
            LOG_DEBUG("start: %s", TimePointToString(originalEndDate + date::days{0}).c_str());
            LOG_DEBUG("start is: %s",
                      TimePointToString(TimePointFromTimeT(TimePointToTimeT(originalStartDate))).c_str());
            LOG_DEBUG("start is: %s", TimePointToString(TimePointFromTimeT(TimePointToTimeT(originalEndDate))).c_str());

            check_custom_repeat(customRepeatOption, numberOfEvents, originalStartDate, originalEndDate);
        }

        SECTION("Repeat Mondays, Tuesdays, Wednesdays, Sundays (original thursday)")
        {
            uint32_t customRepeatOption =
                static_cast<uint32_t>(weekDayOption::monday) + static_cast<uint32_t>(weekDayOption::wednesday) +
                static_cast<uint32_t>(weekDayOption::tuesday) + static_cast<uint32_t>(weekDayOption::sunday);
            uint32_t numberOfEvents     = 17;
            TimePoint originalStartDate = TimePointFromString("2020-12-10 14:30:00"); // thursday
            TimePoint originalEndDate   = TimePointFromString("2020-12-10 15:30:00"); // thursday

            check_custom_repeat(customRepeatOption, numberOfEvents, originalStartDate, originalEndDate);
        }

        SECTION("Repeat Saturdays (original thursday)")
        {
            uint32_t customRepeatOption = static_cast<uint32_t>(weekDayOption::saturday);
            uint32_t numberOfEvents     = 5;
            TimePoint originalStartDate = TimePointFromString("2020-12-10 14:30:00"); // thursday
            TimePoint originalEndDate   = TimePointFromString("2020-12-10 15:30:00"); // thursday

            check_custom_repeat(customRepeatOption, numberOfEvents, originalStartDate, originalEndDate);
        }
    }

    SECTION("Check count from filter")
    {
        TimePoint from = TimePointFromString("2019-10-20 14:30:00");
        TimePoint till = TimePointFromString("2019-10-24 14:20:00");

        CHECK(eventsTbl.countFromFilter(from, till) == 4);
    }

    SECTION("Check get by UID")
    {
        /// get
        auto entry = eventsTbl.getByUID(testRow2.UID);
        check_tableRow(entry, testRow2);
    }

    SECTION("Check Add entry and Get entry by ID")
    {
        CHECK(eventsTbl.count() == 6);

        /// get
        auto entry = eventsTbl.getById(testRow2.ID);
        check_tableRow(entry, testRow2);

        auto entry2 = eventsTbl.getById(testRow4.ID);
        check_tableRow(entry2, testRow4);
    }

    SECTION("Remove entry by ID")
    {
        CHECK(eventsTbl.count() == 6);
        CHECK(eventsTbl.removeById(testRow2.ID));
        CHECK(eventsTbl.count() == 5);
        CHECK_NOTHROW(eventsTbl.getById(testRow2.ID));
    }

    SECTION("Remove entry by UID")
    {
        CHECK(eventsTbl.count() == 6);
        CHECK(eventsTbl.removeByUID(testRow2.UID));
        CHECK(eventsTbl.count() == 5);
        CHECK_NOTHROW(eventsTbl.getById(testRow2.ID));
    }

    SECTION("Update entry by ID")
    {
        CHECK(eventsTbl.count() == 6);

        std::string newTitle = "Updated Title", newProviderID = "PurePhoneUpdated";
        TimePoint newDateFrom    = TimePointFromString("2020-10-20 15:00:00"),
                  newDateTill    = TimePointFromString("2020-10-20 16:00:00");
        uint32_t newReminder     = static_cast<uint32_t>(Reminder::one_week_before);
        uint32_t newRepeatOption = static_cast<uint32_t>(Repeat::biweekly);

        /// check title and provider id update
        auto entryToUpdate        = eventsTbl.getById(testRow6.ID);
        entryToUpdate.title       = newTitle;
        entryToUpdate.provider_id = newProviderID;
        CHECK(eventsTbl.update(entryToUpdate));

        auto entry = eventsTbl.getById(entryToUpdate.ID);
        check_tableRow(entry, entryToUpdate);

        CHECK(eventsTbl.count() == 6);

        /// check date update
        entryToUpdate           = eventsTbl.getById(testRow5.ID);
        entryToUpdate.date_from = newDateFrom;
        entryToUpdate.date_till = newDateTill;
        CHECK(eventsTbl.update(entryToUpdate));

        entry = eventsTbl.getById(entryToUpdate.ID);
        CHECK(entry.ID == entryToUpdate.ID);
        CHECK(entry.UID == entryToUpdate.UID);
        CHECK(entry.title == entryToUpdate.title);
        CHECK(TimePointToString(entry.date_from) == TimePointToString(newDateFrom));
        CHECK(TimePointToString(entry.date_till) == TimePointToString(newDateTill));
        CHECK(entry.reminder == entryToUpdate.reminder);
        CHECK(entry.repeat == entryToUpdate.repeat);
        CHECK(entry.reminder_fired == entryToUpdate.reminder_fired);
        CHECK(entry.provider_type == entryToUpdate.provider_type);
        CHECK(entry.provider_id == entryToUpdate.provider_id);
        CHECK(entry.provider_iCalUid == entryToUpdate.provider_iCalUid);
        CHECK(entry.isValid());

        CHECK(eventsTbl.count() == 6);

        /// check options update
        entryToUpdate          = eventsTbl.getById(testRow5.ID);
        entryToUpdate.repeat   = newRepeatOption;
        entryToUpdate.reminder = newReminder;
        CHECK(eventsTbl.update(entryToUpdate));

        entry = eventsTbl.getById(entryToUpdate.ID);
        check_tableRow(entry, entryToUpdate);

        CHECK(eventsTbl.count() == 6);
    }

    SECTION("Update entry by ID")
    {
        CHECK(eventsTbl.count() == 6);

        std::string newTitle = "Updated Title", newProviderType = "PurePhoneUpdate", newProviderID = "newID",
                    newProvideriCalUid = "new iCalUid";
        TimePoint newDateFrom          = TimePointFromString("2020-10-20 15:00:00"),
                  newDateTill          = TimePointFromString("2020-10-20 16:00:00");
        uint32_t newReminder           = static_cast<uint32_t>(Reminder::one_week_before);
        uint32_t newRepeatOption       = static_cast<uint32_t>(Repeat::biweekly);

        /// check title and provider id update
        auto entryToUpdate             = eventsTbl.getById(testRow6.ID);
        entryToUpdate.title            = newTitle;
        entryToUpdate.provider_type    = newProviderType;
        entryToUpdate.provider_id      = newProviderID;
        entryToUpdate.provider_iCalUid = newProvideriCalUid;
        CHECK(eventsTbl.update(entryToUpdate));

        auto entry = eventsTbl.getById(entryToUpdate.ID);
        check_tableRow(entry, entryToUpdate);

        CHECK(eventsTbl.count() == 6);

        /// check date update
        entryToUpdate           = eventsTbl.getById(testRow5.ID);
        entryToUpdate.date_from = newDateFrom;
        entryToUpdate.date_till = newDateTill;
        CHECK(eventsTbl.update(entryToUpdate));

        entry = eventsTbl.getById(entryToUpdate.ID);
        CHECK(entry.ID == entryToUpdate.ID);
        CHECK(entry.UID == entryToUpdate.UID);
        CHECK(entry.title == entryToUpdate.title);
        CHECK(TimePointToString(entry.date_from) == TimePointToString(newDateFrom));
        CHECK(TimePointToString(entry.date_till) == TimePointToString(newDateTill));
        CHECK(entry.reminder == entryToUpdate.reminder);
        CHECK(entry.repeat == entryToUpdate.repeat);
        CHECK(entry.reminder_fired == entryToUpdate.reminder_fired);
        CHECK(entry.provider_type == entryToUpdate.provider_type);
        CHECK(entry.provider_id == entryToUpdate.provider_id);
        CHECK(entry.provider_iCalUid == entryToUpdate.provider_iCalUid);
        CHECK(entry.isValid());

        CHECK(eventsTbl.count() == 6);

        /// check options update
        entryToUpdate          = eventsTbl.getById(testRow5.ID);
        entryToUpdate.repeat   = newRepeatOption;
        entryToUpdate.reminder = newReminder;
        CHECK(eventsTbl.update(entryToUpdate));

        entry = eventsTbl.getById(entryToUpdate.ID);
        check_tableRow(entry, entryToUpdate);

        CHECK(eventsTbl.count() == 6);
    }

    SECTION("Update entry by UID")
    {
        CHECK(eventsTbl.count() == 6);

        std::string newTitle = "Updated Title", newProviderType = "PurePhoneUpdate", newProviderID = "newID",
                    newProvideriCalUid = "new iCalUid";
        TimePoint newDateFrom          = TimePointFromString("2020-10-20 15:00:00"),
                  newDateTill          = TimePointFromString("2020-10-20 16:00:00");
        uint32_t newReminder           = static_cast<uint32_t>(Reminder::one_week_before);
        uint32_t newRepeatOption       = static_cast<uint32_t>(Repeat::biweekly);

        /// check title and provider id update
        auto entryToUpdate             = eventsTbl.getByUID(testRow6.UID);
        entryToUpdate.title            = newTitle;
        entryToUpdate.provider_type    = newProviderType;
        entryToUpdate.provider_id      = newProviderID;
        entryToUpdate.provider_iCalUid = newProvideriCalUid;
        CHECK(eventsTbl.updateByUID(entryToUpdate));

        auto entry = eventsTbl.getById(entryToUpdate.ID);
        check_tableRow(entry, entryToUpdate);
        CHECK(entry.isValid());

        CHECK(eventsTbl.count() == 6);

        /// check date update
        entryToUpdate           = eventsTbl.getByUID(testRow5.UID);
        entryToUpdate.date_from = newDateFrom;
        entryToUpdate.date_till = newDateTill;
        CHECK(eventsTbl.updateByUID(entryToUpdate));

        entry = eventsTbl.getById(entryToUpdate.ID);
        CHECK(entry.ID == entryToUpdate.ID);
        CHECK(entry.UID == entryToUpdate.UID);
        CHECK(entry.title == entryToUpdate.title);
        CHECK(entry.date_from == newDateFrom);
        CHECK(TimePointToString(entry.date_from) == "2020-10-20 15:00:00");
        CHECK(entry.date_till == newDateTill);
        CHECK(TimePointToString(entry.date_till) == "2020-10-20 16:00:00");
        CHECK(entry.reminder == entryToUpdate.reminder);
        CHECK(entry.repeat == entryToUpdate.repeat);
        CHECK(entry.reminder_fired == entryToUpdate.reminder_fired);
        CHECK(entry.provider_type == entryToUpdate.provider_type);
        CHECK(entry.provider_id == entryToUpdate.provider_id);
        CHECK(entry.provider_iCalUid == entryToUpdate.provider_iCalUid);
        CHECK(entry.isValid());

        CHECK(eventsTbl.count() == 6);

        /// check options update
        entryToUpdate          = eventsTbl.getByUID(testRow5.UID);
        entryToUpdate.repeat   = newRepeatOption;
        entryToUpdate.reminder = newReminder;
        CHECK(eventsTbl.updateByUID(entryToUpdate));

        entry = eventsTbl.getById(entryToUpdate.ID);
        check_tableRow(entry, entryToUpdate);

        CHECK(eventsTbl.count() == 6);
    }

    SECTION("Select entry by date with limit")
    {
        auto entries = eventsTbl.selectByDatePeriod(testRow1.date_from, testRow6.date_from, 0, 3);
        REQUIRE(entries.size() == 3);
        check_tableRow(entries[0], testRow1);

        check_tableRow(entries[1], testRow2);

        check_tableRow(entries[2], testRow3);
    }

    SECTION("Select entry by date max")
    {
        auto entries = eventsTbl.selectByDatePeriod(
            TimePointFromString("2019-10-21 10:00:00"), TimePointFromString("2019-10-24 10:00:00"), 0, UINT32_MAX);
        CHECK(entries.size() == 3);

        check_tableRow(entries[0], testRow2);

        check_tableRow(entries[1], testRow3);

        check_tableRow(entries[2], testRow4);
    }

    SECTION("Select entry by date invalid")
    {
        auto entries = eventsTbl.selectByDatePeriod(
            TimePointFromString("2000-01-01 00:00:00"), TimePointFromString("2012-12-31 23:59:00"), 0, UINT32_MAX);
        CHECK(entries.size() == 0);
    }

    SECTION("Get all limited by date")
    {
        auto entries = eventsTbl.getLimitOffsetByDate(0, 6);
        CHECK(entries.size() == 6);
    }

    SECTION("Select first upcoming event")
    {
        if (eventsTbl.count() > 0) {
            REQUIRE(remove_events(eventsDb));
        }
        CHECK(eventsTbl.count() == 0);

        TimePoint startDate1 = TimePointFromString("2018-10-20 14:24:00");
        TimePoint startDate2 = TimePointFromString("2020-10-20 14:24:00");

        TimePoint tillDate  = TimePointFromString("2030-10-20 15:24:00");
        TimePoint firedDate = TimePointFromString("2018-10-20 14:24:00");

        EventsTableRow testEvent1 = {{1},
                                     .UID              = "test1",
                                     .title            = "Event1",
                                     .date_from        = TimePointFromString("2017-10-19 14:24:00"),
                                     .date_till        = tillDate,
                                     .reminder         = 120,
                                     .repeat           = 0,
                                     .reminder_fired   = TIME_POINT_INVALID,
                                     .provider_type    = "PurePhone",
                                     .provider_id      = "testID",
                                     .provider_iCalUid = "test6"};

        EventsTableRow testEvent2 = {{2},
                                     .UID              = "test2",
                                     .title            = "Event2",
                                     .date_from        = TimePointFromString("2019-10-30 14:24:00"),
                                     .date_till        = tillDate,
                                     .reminder         = 120,
                                     .repeat           = 0,
                                     .reminder_fired   = TIME_POINT_INVALID,
                                     .provider_type    = "PurePhone",
                                     .provider_id      = "testID",
                                     .provider_iCalUid = "test6"};

        EventsTableRow testEvent3 = {{3},
                                     .UID              = "test3",
                                     .title            = "Event3",
                                     .date_from        = TimePointFromString("2020-10-20 14:24:00"),
                                     .date_till        = tillDate,
                                     .reminder         = 120,
                                     .repeat           = 0,
                                     .reminder_fired   = firedDate,
                                     .provider_type    = "PurePhone",
                                     .provider_id      = "testID",
                                     .provider_iCalUid = "test6"};

        EventsTableRow testEvent4 = {{4},
                                     .UID              = "test4",
                                     .title            = "Event4",
                                     .date_from        = TimePointFromString("2021-12-20 14:24:00"),
                                     .date_till        = tillDate,
                                     .reminder         = 120,
                                     .repeat           = 0,
                                     .reminder_fired   = firedDate,
                                     .provider_type    = "PurePhone",
                                     .provider_id      = "testID",
                                     .provider_iCalUid = "test6"};

        EventsTableRow testEvent5 = {{5},
                                     .UID              = "test5",
                                     .title            = "Event5",
                                     .date_from        = TimePointFromString("2022-10-20 14:24:00"),
                                     .date_till        = tillDate,
                                     .reminder         = 120,
                                     .repeat           = 0,
                                     .reminder_fired   = TIME_POINT_INVALID,
                                     .provider_type    = "PurePhone",
                                     .provider_id      = "testID",
                                     .provider_iCalUid = "test6"};

        EventsTableRow testEvent6 = {{6},
                                     .UID              = "test6",
                                     .title            = "Event6",
                                     .date_from        = TimePointFromString("2023-10-20 14:24:00"),
                                     .date_till        = tillDate,
                                     .reminder         = 120,
                                     .repeat           = 0,
                                     .reminder_fired   = TIME_POINT_INVALID,
                                     .provider_type    = "PurePhone",
                                     .provider_id      = "testID",
                                     .provider_iCalUid = "test6"};

        CHECK(eventsTbl.add(testEvent1));
        CHECK(eventsTbl.add(testEvent2));
        CHECK(eventsTbl.add(testEvent3));
        CHECK(eventsTbl.add(testEvent4));
        CHECK(eventsTbl.add(testEvent5));
        CHECK(eventsTbl.add(testEvent6));

        auto entries = eventsTbl.SelectFirstUpcoming(startDate1, tillDate);
        CHECK(entries.size() == 1);
        for (auto entry : entries) {
            CHECK(entry.title == "Event2");
        }

        entries = eventsTbl.SelectFirstUpcoming(startDate2, tillDate);
        CHECK(entries.size() == 1);
        for (auto entry : entries) {
            CHECK(entry.title == "Event5");
        }
    }

    eventsTbl.drop();
    Database::deinitialize();
}
