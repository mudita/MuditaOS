// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstring>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <chrono>
#include <regex>

#include "time/time_conversion.hpp"
#include "time/time_locale.hpp"
#include "time/time_conversion_factory.hpp"
#include "mock/TimeSettingsMock.hpp"

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include <catch2/catch.hpp>

using namespace utils::time;
using namespace std::chrono_literals;

class TimeSettings : public TimeSettingsInterface
{
  public:
    bool timeFormat12h  = true;
    bool dateFormatDDMM = true;

    bool isTimeFormat12h() const final
    {
        return timeFormat12h;
    }
    bool isDateFormatDDMM() const final
    {
        return dateFormatDDMM;
    }
};

const std::regex reg12h("^(0[0-9]|1[0-2]):[0-5][0-9] (A|P)M$");
const std::regex reg12hShort("^(0[0-9]|1[0-2]):[0-5][0-9]$");
const std::regex reg24h("^([0-1][0-9]|2[0-4]):[0-5][0-9]$");
const std::regex regexDDMMYYYY("^([0-2]\\d|3[0-1])\\.(0[1-9]|1[0-2])\\.\\d{4}$");
const std::regex regexDDMM("^([0-2]\\d|3[0-1])\\.(0[1-9]|1[0-2])$");
const std::regex regexMMDDYYYY("^(0[1-9]|1[0-2])\\.([0-2]\\d|3[0-1])\\.\\d{4}$");
const std::regex regexMMDD("^(0[1-9]|1[0-2])\\.([0-2]\\d|3[0-1])$");

TEST_CASE("TimeStamp")
{
    utils::setDisplayLanguage("English");
    setenv("TZ", "GMT0", 1);

    SECTION("default")
    {
        Timestamp timestamp;

        SECTION("no format specified")
        {
            REQUIRE(timestamp.str() == "");
        }
        SECTION("format specified")
        {
            REQUIRE(timestamp.str("%m.%d.%y %H:%M") == "01.01.70 00:00");
            REQUIRE(timestamp.str() == "");
        }

        SECTION("set format")
        {
            timestamp.set_format("%m.%d.%Y %H:%M:%S %a %A %b %B %Z");
            REQUIRE(timestamp.str() == "01.01.1970 00:00:00 Thu Thursday Jan January GMT");
        }
        SECTION("set time")
        {
            timestamp.set_time(1623251054);
            REQUIRE(timestamp.str("%m.%d.%y %H:%M:%S") == "06.09.21 15:04:14");
        }
        SECTION("format too long")
        {
            REQUIRE(timestamp.str("%m.%d.%Y %H:%M:%S %a %A %b %B %Z %m.%d.%Y %H:%M:%S %a %A %b %B %Z %m.%d.%Y %H:%M:%S "
                                  "%a %A %b %B %Z ") == "");
        }
        SECTION("timestamp invalid")
        {
            Timestamp timestamp(static_cast<time_t>(1));
            REQUIRE(timestamp.str("%m.%d.%y %H:%M") == "01.01.70 00:00");
        }
    }

    SECTION("timestamp set")
    {
        // Epoch timestamp: 1623714101
        // Timestamp in milliseconds: 1623670901000
        // Date and time (GMT): Monday, June 14, 2021 11:41:41 PM
        // Date and time (CET): Tuesday, June 15, 2021 1:41:41 AM GMT+02:00
        Timestamp timestamp(1623714101);
        setenv("TZ", "GMT0", 1);

        SECTION("no format specified")
        {
            REQUIRE(timestamp.str() == "");
        }
        SECTION("format specified")
        {
            REQUIRE(timestamp.str("%m.%d.%y %H:%M:%S") == "06.14.21 23:41:41");
            REQUIRE(timestamp.str(Locale::format(Locale::TimeFormat::FormatTime12H)) == "11:41 PM");
            REQUIRE(timestamp.str(Locale::format(Locale::TimeFormat::FormatTime12HShort)) == "11:41");
            REQUIRE(timestamp.str(Locale::format(Locale::TimeFormat::FormatTime24H)) == "23:41");
            REQUIRE(timestamp.str(Locale::format(Locale::TimeFormat::FormatLocaleDate_DD_MM_YYYY)) == "14.06.2021");
            REQUIRE(timestamp.str(Locale::format(Locale::TimeFormat::FormatLocaleDate_MM_DD_YYYY)) == "06.14.2021");
            REQUIRE(timestamp.str(Locale::format(Locale::TimeFormat::FormatLocaleDate_DD_MM)) == "14.06");
            REQUIRE(timestamp.str(Locale::format(Locale::TimeFormat::FormatLocaleDate_MM_DD)) == "06.14");
        }

        SECTION("day and month")
        {
            // Epoch timestamp: 1609750906
            // Timestamp in milliseconds: 1609750906000
            // Date and time (GMT): Monday, January 4, 2021 9:01:46 AM
            // Date and time (your time zone): Monday, January 4, 2021 10:01:46 AM GMT+01:00
            const time_t currentTime   = 1609750906;
            const auto currentTimeinfo = *std::localtime(&currentTime);

            SECTION("day")
            {
                const std::vector<std::string> days = {
                    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
                const std::vector<std::string> daysAbrev = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

                for (auto i = 0; i < 7; i++) {
                    auto newTimeinfo = currentTimeinfo;
                    newTimeinfo.tm_mday += i;
                    timestamp.set_time(std::mktime(&newTimeinfo));
                    REQUIRE(timestamp.day(false) == days[i]);
                    REQUIRE(timestamp.day(true) == daysAbrev[i]);
                }
            }

            SECTION("month")
            {
                const std::vector<std::string> months      = {"January",
                                                         "February",
                                                         "March",
                                                         "April",
                                                         "May",
                                                         "June",
                                                         "July",
                                                         "August",
                                                         "September",
                                                         "October",
                                                         "November",
                                                         "December"};
                const std::vector<std::string> monthsAbrev = {
                    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

                for (auto i = 0; i < 12; i++) {
                    auto newTimeinfo = currentTimeinfo;
                    newTimeinfo.tm_mon += i;
                    timestamp.set_time(std::mktime(&newTimeinfo));
                    REQUIRE(timestamp.month(false) == months[i]);
                    REQUIRE(timestamp.month(true) == monthsAbrev[i]);
                }
            }
        }

        SECTION("Timezone change")
        {
            REQUIRE(timestamp.str("%d.%m.%y %H:%M:%S %Z") == "14.06.21 23:41:41 GMT");
            setenv("TZ", "ABC-1", 1);
            REQUIRE(timestamp.str("%d.%m.%y %H:%M:%S %Z") == "15.06.21 00:41:41 ABC");
            setenv("TZ", "CBA1", 1);
            REQUIRE(timestamp.str("%d.%m.%y %H:%M:%S %Z") == "14.06.21 22:41:41 CBA");
        }
    }
}

TEST_CASE("DateTime")
{
    utils::setDisplayLanguage("English");
    auto timezone = GENERATE("GMTO", "GMT-2", "GMT+2");
    setenv("TZ", timezone, 1);
    TimeSettings timeSettings;

    const time_t currentTime       = GENERATE(1623714101, /* Monday, June 14, 2021 11:41:41 PM GMT  */
                                        1625084493, /* Wednesday, June 30, 2021 8:21:33 PM GMT */
                                        1625170893, /* Thursday, July 1, 2021 8:21:33 PM GMT */
                                        1625257293, /* Friday, July 2, 2021 8:21:33 PM GMT */
                                        1640982093, /* Friday, December 31, 2021 8:21:33 PM GMT */
                                        1641068493, /* Saturday, January 1, 2022 8:21:33 PM GMT */
                                        1614525589, /* Sunday, February 28, 2021 3:19:49 PM GMT */
                                        1614611989, /* Monday, March 1, 2021 3:19:49 PM GMT */
                                        1614698389  /* Tuesday, March 2, 2021 3:19:49 PM GMT */
    );
    const auto currentTimeTimeinfo = *std::localtime(&currentTime);

    SECTION("isToday")
    {
        SECTION("today")
        {
            auto newTimeTimeinfo    = currentTimeTimeinfo;
            auto i                  = GENERATE(range(0, 23));
            newTimeTimeinfo.tm_hour = i;
            auto newTime            = std::mktime(&newTimeTimeinfo);
            DateTime datetime(timeSettings, newTime, currentTime);
            REQUIRE(datetime.isToday());
        }
        SECTION("not today")
        {
            auto newTimeTimeinfo = currentTimeTimeinfo;
            SECTION("different day")
            {
                for (int i = 1; i <= 31; i++) {
                    newTimeTimeinfo.tm_mday = i;
                    auto newTime            = std::mktime(&newTimeTimeinfo);
                    if (newTimeTimeinfo.tm_mday == currentTimeTimeinfo.tm_mday) {
                        continue;
                    }
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(!datetime.isToday());
                }
            }
            SECTION("different month")
            {
                for (int i = 1; i <= 12; i++) {
                    newTimeTimeinfo.tm_mon = i;
                    auto newTime           = std::mktime(&newTimeTimeinfo);
                    if (newTimeTimeinfo.tm_mon == currentTimeTimeinfo.tm_mon) {
                        continue;
                    }
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(!datetime.isToday());
                }
            }
            SECTION("previous years")
            {
                auto i                  = GENERATE(range(1, 100));
                newTimeTimeinfo.tm_year = currentTimeTimeinfo.tm_year - i;
                auto newTime            = std::mktime(&newTimeTimeinfo);
                DateTime datetime(timeSettings, newTime, currentTime);
                REQUIRE(!datetime.isToday());
            }
            SECTION("next years")
            {
                auto i                  = GENERATE(range(1, 100));
                newTimeTimeinfo.tm_year = currentTimeTimeinfo.tm_year + i;
                auto newTime            = std::mktime(&newTimeTimeinfo);
                DateTime datetime(timeSettings, newTime, currentTime);
                REQUIRE(!datetime.isToday());
            }
        }
    }

    SECTION("isYesterday")
    {
        auto newTimeTimeinfo = currentTimeTimeinfo;
        SECTION("yesterday")
        {
            newTimeTimeinfo.tm_mday -= 1;
            for (int i = 0; i < 24; i++) {
                newTimeTimeinfo.tm_hour = i;
                auto newTime            = std::mktime(&newTimeTimeinfo);
                DateTime datetime(timeSettings, newTime, currentTime);
                REQUIRE(datetime.isYesterday());
            }
        }
        SECTION("not yesterday")
        {
            SECTION("different days")
            {
                for (int i = 1; i < 32; i++) {
                    newTimeTimeinfo.tm_mday = i;
                    auto newTime            = std::mktime(&newTimeTimeinfo);
                    if (newTimeTimeinfo.tm_mday == currentTimeTimeinfo.tm_mday - 1) {
                        continue;
                    }
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(!datetime.isYesterday());
                }
            }
            SECTION("different months")
            {
                for (int i = 0; i < 12; i++) {
                    newTimeTimeinfo.tm_mon = i;
                    auto newTime           = std::mktime(&newTimeTimeinfo);
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(!datetime.isYesterday());
                }
            }
            SECTION("different years")
            {
                // previous
                for (int i = 1; i < 100; i++) {
                    newTimeTimeinfo.tm_year = currentTimeTimeinfo.tm_year - i;
                    auto newTime            = std::mktime(&newTimeTimeinfo);
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(!datetime.isYesterday());
                }
                // forward
                for (int i = 1; i < 100; i++) {
                    newTimeTimeinfo.tm_year = currentTimeTimeinfo.tm_year + i;
                    auto newTime            = std::mktime(&newTimeTimeinfo);
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(!datetime.isYesterday());
                }
            }
        }

        SECTION("isCurrentYear")
        {
            auto newTimeTimeinfo = currentTimeTimeinfo;
            SECTION("different days")
            {
                for (int i = 1; i < 32; i++) {
                    newTimeTimeinfo.tm_mday = i;
                    auto newTime            = std::mktime(&newTimeTimeinfo);
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(datetime.isCurrentYear());
                }
            }
            SECTION("different months")
            {
                for (int i = 0; i < 12; i++) {
                    newTimeTimeinfo.tm_mon = i;
                    auto newTime           = std::mktime(&newTimeTimeinfo);
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(datetime.isCurrentYear());
                }
            }
            SECTION("different years")
            {
                // previous
                for (int i = 1; i < 100; i++) {
                    newTimeTimeinfo.tm_year = currentTimeTimeinfo.tm_year - i;
                    auto newTime            = std::mktime(&newTimeTimeinfo);
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(!datetime.isCurrentYear());
                }
                // forward
                for (int i = 1; i < 100; i++) {
                    newTimeTimeinfo.tm_year = currentTimeTimeinfo.tm_year + i;
                    auto newTime            = std::mktime(&newTimeTimeinfo);
                    DateTime datetime(timeSettings, newTime, currentTime);
                    REQUIRE(!datetime.isCurrentYear());
                }
            }
        }
    }

    SECTION("Display")
    {
        auto newTimeTimeinfo        = currentTimeTimeinfo;
        timeSettings.timeFormat12h  = true;
        timeSettings.dateFormatDDMM = true;
        SECTION("now")
        {
            std::regex reg = reg12h;
            auto newTime   = std::mktime(&newTimeTimeinfo);
            DateTime datetime(timeSettings, newTime, currentTime);

            REQUIRE(std::regex_match(std::string(datetime.str()), reg));
            timeSettings.timeFormat12h = false;
            reg                        = reg24h;
            REQUIRE(std::regex_match(std::string(datetime.str()), reg));
        }

        SECTION("yesterday")
        {
            newTimeTimeinfo.tm_mday -= 1;
            auto newTime = std::mktime(&newTimeTimeinfo);
            DateTime datetime(timeSettings, newTime, currentTime);

            REQUIRE(datetime.str() == "Yesterday");
        }

        SECTION("same year")
        {
            std::regex regexSameYear = regexDDMM;
            if (newTimeTimeinfo.tm_mon == 0) {
                newTimeTimeinfo.tm_mon = 1;
            }
            else {
                newTimeTimeinfo.tm_mon -= 1;
            }
            auto newTime = std::mktime(&newTimeTimeinfo);

            DateTime datetime(timeSettings, newTime, currentTime);
            REQUIRE(std::regex_match(std::string(datetime.str()), regexSameYear));
            timeSettings.dateFormatDDMM = false;
            regexSameYear               = regexMMDD;
            REQUIRE(std::regex_match(std::string(datetime.str()), regexSameYear));
        }

        SECTION("different year")
        {
            std::regex regexPreviousYear = regexDDMMYYYY;
            newTimeTimeinfo.tm_year -= 1;
            auto newTime = std::mktime(&newTimeTimeinfo);

            DateTime datetime(timeSettings, newTime, currentTime);

            REQUIRE(std::regex_match(std::string(datetime.str()), regexPreviousYear));
            timeSettings.dateFormatDDMM = false;
            regexPreviousYear           = regexMMDDYYYY;
            REQUIRE(std::regex_match(std::string(datetime.str()), regexPreviousYear));
        }
    }
}

TEST_CASE("Time")
{
    utils::setDisplayLanguage("English");
    setenv("TZ", "GMT0", 1);
    TimeSettings timeSettings;

    SECTION("Display")
    {
        const auto currentTime = 1623714101; // Monday, June 14, 2021 11:41:41 PM GMT

        timeSettings.timeFormat12h  = true;
        timeSettings.dateFormatDDMM = true;
        SECTION("now")
        {
            std::regex reg = reg12h;
            Time time(timeSettings, currentTime);

            REQUIRE(std::regex_match(std::string(time.str()), reg));
            timeSettings.timeFormat12h = false;
            reg                        = reg24h;
            REQUIRE(std::regex_match(std::string(time.str()), reg));
        }
    }
}

TEST_CASE("Clock")
{
    utils::setDisplayLanguage("English");
    setenv("TZ", "GMT0", 1);
    TimeSettings timeSettings;

    SECTION("Display")
    {
        const auto currentTime = 1623714101; // Monday, June 14, 2021 11:41:41 PM GMT

        timeSettings.timeFormat12h  = true;
        timeSettings.dateFormatDDMM = true;
        SECTION("now")
        {
            std::regex reg = reg12hShort;
            Clock clock(timeSettings, currentTime);

            REQUIRE(std::regex_match(std::string(clock.str()), reg));
            timeSettings.timeFormat12h = false;
            reg                        = reg24h;
            REQUIRE(std::regex_match(std::string(clock.str()), reg));
        }
    }
}

TEST_CASE("Date")
{
    utils::setDisplayLanguage("English");
    setenv("TZ", "GMT0", 1);
    TimeSettings timeSettings;

    SECTION("Display")
    {
        const auto currentTime = 1623714101; // Monday, June 14, 2021 11:41:41 PM GMT

        timeSettings.timeFormat12h  = true;
        timeSettings.dateFormatDDMM = true;
        SECTION("now")
        {
            std::regex reg = regexDDMMYYYY;
            Date date(timeSettings, currentTime);

            REQUIRE(std::regex_match(std::string(date.str()), reg));
            timeSettings.dateFormatDDMM = false;
            reg                         = regexMMDDYYYY;
            REQUIRE(std::regex_match(std::string(date.str()), reg));
        }
    }
}

TEST_CASE("DateText")
{
    utils::setDisplayLanguage("English");
    setenv("TZ", "GMT0", 1);
    TimeSettings timeSettings;

    SECTION("Display")
    {
        // Epoch timestamp: 1623714101
        // Timestamp in milliseconds: 1623670901000
        // Date and time (GMT): Monday, June 14, 2021 11:41:41 PM
        // Date and time (CET): Tuesday, June 15, 2021 1:41:41 AM GMT+02:00
        DateText timestamp(timeSettings, 1623714101);
        setenv("TZ", "GMT0", 1);

        timeSettings.timeFormat12h  = true;
        timeSettings.dateFormatDDMM = true;
        SECTION("now")
        {
            REQUIRE(timestamp.str() == "Monday, 14 Jun");
            timeSettings.dateFormatDDMM = false;
            REQUIRE(timestamp.str() == "Monday, Jun 14");
        }
    }
}

TEST_CASE("Duration - creation")
{
    utils::setDisplayLanguage("English");

    SECTION("default constructor")
    {
        utils::time::Duration duration;
        REQUIRE(duration.get() == 0);
    }

    SECTION("time_t constructor")
    {
        utils::time::Duration duration(333);
        REQUIRE(duration.get() == 333);
    }

    SECTION("stop < start time_t constructor")
    {
        utils::time::Duration duration(100, 202);
        REQUIRE(duration.get() == 0);
    }

    SECTION("stop > start time_t constructor")
    {
        utils::time::Duration duration(202, 102);
        REQUIRE(duration.get() == 100);
    }

    SECTION("Start and Stop Timestamp constructor")
    {
        utils::time::Timestamp tstart(100);
        utils::time::Timestamp tstop(202);
        {
            utils::time::Duration duration(tstop, tstart);
            REQUIRE(duration.get() == 102);
        }
        {
            utils::time::Duration duration(tstart, tstop);
            REQUIRE(duration.get() == 0);
        }
    }
}

TEST_CASE("Duration - arithemtics")
{
    utils::setDisplayLanguage("English");

    SECTION("Addition")
    {
        utils::time::Duration d1(100), d2(200);
        utils::time::Timestamp t1(100);
        auto t2 = t1 + d1;
        REQUIRE(t2.getTime() == 200);

        auto t3 = d1 + t1;
        REQUIRE(t3.getTime() == 200);

        auto d3 = d1 + d2;
        REQUIRE(d3.get() == 300);

        auto d4 = d1 + utils::time::Duration(5);
        REQUIRE(d4.get() == 105);
    }
    SECTION("Substraction")
    {
        utils::time::Duration d1(1000);
        utils::time::Timestamp t1(100), t2(205);
        auto d2 = t1 - t2;
        REQUIRE(d2.get() == 0);

        auto d3 = t2 - t1;
        REQUIRE(d3.get() == 105);

        auto d4 = d1 - d3;
        REQUIRE(d4.get() == 895);

        auto d5 = d3 - d1;
        REQUIRE(d5.get() == 0);
    }
}

TEST_CASE("Duration - comparision")
{
    utils::setDisplayLanguage("English");

    SECTION("Duration")
    {
        utils::time::Duration d1(100), d2(200), d3(200);
        REQUIRE(!(d1 == d3));
        REQUIRE(d2 == d3);

        REQUIRE(d1 != d3);
        REQUIRE(!(d2 != d3));

        REQUIRE(d1 < d2);
        REQUIRE(!(d2 < d3));
        REQUIRE(d2 > d1);
        REQUIRE(!(d2 > d3));

        REQUIRE(d1 <= d2);
        REQUIRE(d2 <= d3);
        REQUIRE(d2 >= d1);
        REQUIRE(d2 >= d3);
    }
    SECTION("Timestamps")
    {
        utils::time::Timestamp t1(100), t2(200), t3(200);
        REQUIRE(!(t1 == t3));
        REQUIRE(t2 == t3);

        REQUIRE(t1 != t3);
        REQUIRE(!(t2 != t3));

        REQUIRE(t1 < t2);
        REQUIRE(!(t2 < t3));
        REQUIRE(t2 > t1);
        REQUIRE(!(t2 > t3));

        REQUIRE(t1 <= t2);
        REQUIRE(t2 <= t3);
        REQUIRE(t2 >= t1);
        REQUIRE(t2 >= t3);
    }
}

TEST_CASE("Duration - display")
{
    utils::setDisplayLanguage("English");

    {
        using namespace utils::time;
        Duration duration;
        REQUIRE(duration.get() == 0);

        duration = 0;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "00:00");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "0:00");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "00:00");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:00:00");
        duration = 8;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "00:08");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "0:08");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "00:08");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:00:08");
        duration = 20;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "00:20");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "0:20");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "00:20");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:00:20");
        duration = 1 * 60 + 59;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "01:59");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "1:59");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "01:59");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:01:59");
        duration = 2 * 60 + 30;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "02:30");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "2:30");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "02:30");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:02:30");
        duration = 19 * 60 + 32;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "19:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "19:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "19:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:19:32");
        duration = 1 * 60 * 60 + 6 * 60 + 9;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "1:06:09");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "1:06:09");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "66:09");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "1:06:09");
        duration = 1 * 60 * 60 + 29 * 60 + 32;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "1:29:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "1:29:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "89:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "1:29:32");
        duration = 27 * 60 * 60 + 23 * 60 + 14;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "27:23:14");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "27:23:14");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "1643:14");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "27:23:14");
        duration = 48 * 60 * 60 + 3 * 60 + 4;
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "48:03:04");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "48:03:04");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "2883:04");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "48:03:04");
    }
}
TEST_CASE("Timestamp factory")
{
    SECTION("No setting provided")
    {
        auto timestamp = utils::time::TimestampFactory().createTimestamp(utils::time::TimestampType::Clock);

        REQUIRE(typeid(*(timestamp.get())).name() == typeid(utils::time::Timestamp).name());
    }
    SECTION("Settings provided")
    {
        auto factory = utils::time::TimestampFactory();
        factory.init(new BaseTimeSettings);
        auto timestamp = factory.createTimestamp(utils::time::TimestampType::Clock);

        REQUIRE(typeid(*(timestamp.get())).name() == typeid(utils::time::Clock).name());
    }
}
