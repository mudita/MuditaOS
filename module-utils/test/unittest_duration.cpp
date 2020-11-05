// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstring>
#include <iostream>
#include <memory>
#include <vfs.hpp>
#include <unistd.h>

#include "time/time_conversion.hpp"

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include <catch2/catch.hpp>

class vfs vfs;
utils::i18n localize;

struct vfs_initializer
{
    vfs_initializer()
    {
        vfs.Init();
    }
} vfs_intializer;

TEST_CASE("Duration - creation")
{
    utils::localize.setDisplayLanguage("English");

    SECTION("default constructor")
    {
        utils::time::Duration duration;
        REQUIRE(duration.get() == std::chrono::seconds::zero());
    }

    SECTION("size_t constructor")
    {
        utils::time::Duration duration(std::chrono::seconds(333));
        REQUIRE(duration.get() == std::chrono::seconds(333));
    }

    SECTION("stop < start size_t constructor")
    {
        utils::time::Duration duration(utils::time::TimeConversion::toTimePoint(100), utils::time::TimeConversion::toTimePoint(202));
        REQUIRE(duration.get() == std::chrono::seconds::zero());
    }

    SECTION("stop > start size_t constructor")
    {
        utils::time::Duration duration(utils::time::TimeConversion::toTimePoint(202), utils::time::TimeConversion::toTimePoint(102));
        REQUIRE(duration.get() == std::chrono::seconds(100));
    }

    SECTION("Start and Stop Timestamp constructor")
    {
        utils::time::Timestamp tstart(utils::time::TimeConversion::toTimePoint(100));
        utils::time::Timestamp tstop(utils::time::TimeConversion::toTimePoint(202));
        {
            utils::time::Duration duration(tstop, tstart);
            REQUIRE(duration.get() == std::chrono::seconds(102));
        }
        {
            utils::time::Duration duration(tstart, tstop);
            REQUIRE(duration.get() == std::chrono::seconds::zero());
        }
    }
}

TEST_CASE("Duration - arithemtics")
{
    utils::localize.setDisplayLanguage("English");

    SECTION("Addition")
    {
        utils::time::Duration d1(std::chrono::seconds(100)), d2(std::chrono::seconds(200));
        utils::time::Timestamp t1(utils::time::TimeConversion::toTimePoint(100), utils::time::TimestampType::use_as_localtime);
        auto t2 = t1 + d1;
        REQUIRE(t2.getLocalTime() == utils::time::TimeConversion::toTimePoint(200));

        auto t3 = d1 + t1;
        REQUIRE(t3.getLocalTime() == utils::time::TimeConversion::toTimePoint(200));

        auto d3 = d1 + d2;
        REQUIRE(d3.get() == std::chrono::seconds(300));

        auto d4 = d1 + utils::time::Duration(std::chrono::seconds(5));
        REQUIRE(d4.get() == std::chrono::seconds(105));
    }
    SECTION("Substraction")
    {
        utils::time::Duration d1(std::chrono::seconds(1000));
        utils::time::Timestamp t1(utils::time::TimeConversion::toTimePoint(100)), t2(utils::time::TimeConversion::toTimePoint(205));
        auto d2 = t1 - t2;
        REQUIRE(d2.get() == std::chrono::seconds::zero());

        auto d3 = t2 - t1;
        REQUIRE(d3.get() == std::chrono::seconds(105));

        auto d4 = d1 - d3;
        REQUIRE(d4.get() == std::chrono::seconds(895));

        auto d5 = d3 - d1;
        REQUIRE(d5.get() == std::chrono::seconds::zero());
    }
}

TEST_CASE("Duration - comparision")
{
    utils::localize.setDisplayLanguage("English");

    SECTION("Duration")
    {
        utils::time::Duration d1(std::chrono::seconds(100)), d2(std::chrono::seconds(200)), d3(std::chrono::seconds(200));
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
        utils::time::Timestamp t1(utils::time::TimeConversion::toTimePoint(100)), t2(utils::time::TimeConversion::toTimePoint(200)),
            t3(utils::time::TimeConversion::toTimePoint(200));
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
    utils::localize.setDisplayLanguage("English");

    {
        using namespace utils::time;
        Duration duration;
        REQUIRE(duration.get() == std::chrono::seconds::zero());

        duration = std::chrono::seconds::zero();
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "00:00");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "0:00");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "00:00");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:00:00");
        duration = std::chrono::seconds(8);
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "00:08");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "0:08");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "00:08");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:00:08");
        duration = std::chrono::seconds(20);
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "00:20");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "0:20");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "00:20");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:00:20");
        duration = std::chrono::seconds(1 * 60 + 59);
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "01:59");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "1:59");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "01:59");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:01:59");
        duration = std::chrono::seconds(2 * 60 + 30);
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "02:30");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "2:30");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "02:30");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:02:30");
        duration = std::chrono::seconds(19 * 60 + 32);
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "19:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "19:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "19:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "0:19:32");
        duration = std::chrono::seconds(1 * 60 * 60 + 6 * 60 + 9);
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "1:06:09");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "1:06:09");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "66:09");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "1:06:09");
        duration = std::chrono::seconds(1 * 60 * 60 + 29 * 60 + 32);
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "1:29:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "1:29:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "89:32");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "1:29:32");
        duration = std::chrono::seconds(27 * 60 * 60 + 23 * 60 + 14);
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "27:23:14");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "27:23:14");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "1643:14");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "27:23:14");
        duration = std::chrono::seconds(48 * 60 * 60 + 3 * 60 + 4);
        REQUIRE(duration.str(Duration::DisplayedFormat::Auto0M) == "48:03:04");
        REQUIRE(duration.str(Duration::DisplayedFormat::AutoM) == "48:03:04");
        REQUIRE(duration.str(Duration::DisplayedFormat::Fixed0M0S) == "2883:04");
        REQUIRE(duration.str(Duration::DisplayedFormat::FixedH0M0S) == "48:03:04");
    }
}
