// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneNumber.hpp"
#include "country.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <string>

using namespace utils;

static const std::string pl_entered       = "600123456";
static const std::string pl_e164          = "+48600123456";
static const std::string pl_formatted     = "600 123 456";
static const std::string pl_formatted_int = "+48 600 123 456";

TEST_CASE("PhoneNumber - parsing")
{
    SECTION("PL - valid")
    {
        PhoneNumber number(pl_entered, country::Id::POLAND);
        REQUIRE(number.isValid());
        REQUIRE(number.getCountryCode() == country::Id::POLAND);
    }

    SECTION("Wrong country")
    {
        PhoneNumber number(pl_entered, country::Id::UNITED_KINGDOM);
        REQUIRE_FALSE(number.isValid());
        REQUIRE(number.getCountryCode() == country::Id::UNKNOWN);
    }

    SECTION("From E164")
    {
        PhoneNumber number(pl_e164, country::Id::UNKNOWN);
        REQUIRE(number.isValid());
        REQUIRE(number.getCountryCode() == country::Id::POLAND);
    }

    SECTION("Invalid input")
    {
        PhoneNumber number1("foobar");
        REQUIRE_FALSE(number1.isValid());

        PhoneNumber number2("-1");
        REQUIRE_FALSE(number2.isValid());

        PhoneNumber number3("");
        REQUIRE_FALSE(number3.isValid());
    }

    SECTION("Wrapper")
    {
        const std::string dummy = "12345";

        auto view         = PhoneNumber::parse(pl_e164);
        auto invalid_view = PhoneNumber::parse(dummy);

        REQUIRE(view.isValid());
        REQUIRE(view.getE164() == pl_e164);

        REQUIRE_FALSE(invalid_view.isValid());
        REQUIRE(invalid_view.getE164() == "");
        REQUIRE(invalid_view.getEntered() == dummy);
    }
}

TEST_CASE("PhoneNumber - formatting")
{
    SECTION("From national")
    {
        PhoneNumber number(pl_entered, country::Id::POLAND);
        REQUIRE(number.getFormatted() == pl_formatted);
    }

    SECTION("From international")
    {
        PhoneNumber number(pl_e164, country::Id::UNKNOWN);
        REQUIRE(number.getFormatted() == pl_formatted_int);
    }
}

TEST_CASE("PhoneNumber - views")
{
    SECTION("Invalid")
    {
        const std::string dummyNumber = "12345";
        PhoneNumber number(dummyNumber);
        REQUIRE_FALSE(number.isValid());

        auto view = number.getView();
        REQUIRE(view.getEntered() == dummyNumber);
        REQUIRE(view.getFormatted() == dummyNumber);
        REQUIRE(view.getE164() == "");
    }

    SECTION("Valid")
    {
        PhoneNumber number(pl_entered, country::Id::POLAND);
        REQUIRE(number.isValid());

        auto view = number.getView();
        REQUIRE(view.getEntered() == pl_entered);
        REQUIRE(view.getE164() == pl_e164);
        REQUIRE(view.getFormatted() == pl_formatted);
    }

    SECTION("Safe get non empty number")
    {
        const std::string dummyNumber = "12345";

        auto invalid_view = PhoneNumber::parse(dummyNumber);
        auto valid_view   = PhoneNumber::parse(pl_e164);

        REQUIRE_FALSE(invalid_view.isValid());
        REQUIRE(valid_view.isValid());

        REQUIRE(invalid_view.getNonEmpty() != "");
        REQUIRE(valid_view.getNonEmpty() != "");

        REQUIRE(invalid_view.getNonEmpty() == dummyNumber);
        REQUIRE(valid_view.getNonEmpty() == pl_e164);
    }

    SECTION("View promotion (E164)")
    {
        auto number1 = PhoneNumber(pl_e164);
        auto number2 = PhoneNumber(number1.getView());

        REQUIRE(number1 == number2);
    }

    SECTION("View promotion (non E164)")
    {
        auto number1 = PhoneNumber(pl_entered);
        auto number2 = PhoneNumber(number1.getView());

        REQUIRE(number1 == number2);
    }
}

TEST_CASE("PhoneNumber - equality")
{
    PhoneNumber number1(pl_entered, country::Id::POLAND);
    PhoneNumber number2(pl_e164, country::Id::UNKNOWN);

    REQUIRE(number1 == number1);
    REQUIRE(number2 == number2);

    REQUIRE(number1 == number2);

    REQUIRE(number1 == number2.getView());
    REQUIRE(number2 == number1.getView());
    REQUIRE(number1.getView() == number2.getView());
}

TEST_CASE("PhoneNumber - matching")
{
    PhoneNumber number1(pl_entered, country::Id::POLAND);
    PhoneNumber number2(pl_e164, country::Id::UNKNOWN);
    PhoneNumber number4("600123", country::Id::UNKNOWN);
    PhoneNumber number5("456", country::Id::UNKNOWN);
    PhoneNumber number6("600456", country::Id::UNKNOWN);
    PhoneNumber number7(pl_entered, country::Id::UNKNOWN);

    SECTION("Exact")
    {
        // E164 exact matches national with country code
        REQUIRE(number1.match(number2) == PhoneNumber::Match::EXACT);
        REQUIRE(number2.match(number1) == PhoneNumber::Match::EXACT);

        // national matches national, e164 matches e164
        REQUIRE(number7.match(PhoneNumber(pl_entered, country::Id::UNKNOWN)) == PhoneNumber::Match::EXACT);
        REQUIRE(number2.match(PhoneNumber(pl_e164, country::Id::UNKNOWN)) == PhoneNumber::Match::EXACT);
    }

    SECTION("Possible")
    {
        // possible that national with unknown country matches same national
        // with valid country
        REQUIRE(number7.match(number1) == PhoneNumber::Match::POSSIBLE);
        REQUIRE(number1.match(number7) == PhoneNumber::Match::POSSIBLE);
        REQUIRE(number7.match(number2) == PhoneNumber::Match::POSSIBLE);
        REQUIRE(number2.match(number7) == PhoneNumber::Match::POSSIBLE);
    }

    SECTION("Probable")
    {
        REQUIRE(number7.match(number5) == PhoneNumber::Match::PROBABLE);
        REQUIRE(number5.match(number7) == PhoneNumber::Match::PROBABLE);
    }

    SECTION("No match")
    {
        REQUIRE(number1.match(number4) == PhoneNumber::Match::NO_MATCH);
        REQUIRE(number1.match(number6) == PhoneNumber::Match::NO_MATCH);
    }
}

TEST_CASE("PhoneNumber - record validation")
{
    SECTION("Valid")
    {
        auto number = PhoneNumber(pl_entered, pl_e164);
        REQUIRE(number.isValid());
        REQUIRE(number.getFormatted() == pl_formatted);
    }

    SECTION("Valid, both E164")
    {
        auto number = PhoneNumber(pl_e164, pl_e164);
        REQUIRE(number.isValid());
        REQUIRE(number.getFormatted() == pl_formatted_int);
    }

    SECTION("Invalid")
    {
        REQUIRE_THROWS_AS(PhoneNumber(pl_entered, "+44600123456"), utils::PhoneNumber::Error);
        REQUIRE_THROWS_AS(PhoneNumber(pl_entered, pl_entered), utils::PhoneNumber::Error);
    }

    SECTION("Empty E164")
    {
        REQUIRE_NOTHROW(PhoneNumber(pl_entered, ""));
        auto number = PhoneNumber(pl_entered, "");
        REQUIRE(number.get() == pl_entered);
        REQUIRE(number.getFormatted() == pl_entered);
        REQUIRE_FALSE(number.isValid());
    }

    SECTION("Entered single e164")
    {
        REQUIRE_NOTHROW(PhoneNumber(pl_e164, ""));
        auto number = PhoneNumber(pl_e164, pl_e164);
        REQUIRE(number.isValid());
        REQUIRE(number.getFormatted() == pl_formatted_int);
        REQUIRE(number.getCountryCode() == country::Id::POLAND);
    }
}
