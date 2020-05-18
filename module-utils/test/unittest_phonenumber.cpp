#include "PhoneNumber.hpp"
#include "country.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

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
    }

    SECTION("Wrong country")
    {
        PhoneNumber number(pl_entered, country::Id::UNITED_KINGDOM);
        REQUIRE_FALSE(number.isValid());
    }

    SECTION("From E164")
    {
        PhoneNumber number(pl_e164, country::Id::UNKNOWN);
        REQUIRE(number.isValid());
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

        auto view = number.makeView();
        REQUIRE(view.getEntered() == dummyNumber);
        REQUIRE(view.getFormatted() == dummyNumber);
        REQUIRE(view.getE164() == "");
    }

    SECTION("Valid")
    {
        PhoneNumber number(pl_entered, country::Id::POLAND);
        REQUIRE(number.isValid());

        auto view = number.makeView();
        REQUIRE(view.getEntered() == pl_entered);
        REQUIRE(view.getE164() == pl_e164);
        REQUIRE(view.getFormatted() == pl_formatted);
    }

    SECTION("Safe get non empty number")
    {
        const std::string dummyNumber = "12345";

        auto invalid_view = PhoneNumber::parse(dummyNumber);
        auto valid_view   = PhoneNumber::parse(pl_entered);

        REQUIRE_FALSE(invalid_view.isValid());
        REQUIRE(valid_view.isValid());

        REQUIRE(invalid_view.getNonEmpty() != "");
        REQUIRE(valid_view.getNonEmpty() != "");

        REQUIRE(invalid_view.getNonEmpty() == dummyNumber);
        REQUIRE(valid_view.getNonEmpty() == pl_e164);
    }
}

TEST_CASE("PhoneNumber - equality")
{
    PhoneNumber number1(pl_entered, country::Id::POLAND);
    PhoneNumber number2(pl_e164, country::Id::UNKNOWN);

    REQUIRE(number1 == number1);
    REQUIRE(number2 == number2);

    REQUIRE(number1 == number2);

    REQUIRE(number1 == number2.makeView());
    REQUIRE(number2 == number1.makeView());
    REQUIRE(number1.makeView() == number2.makeView());
}

TEST_CASE("PhoneNumber - record validation")
{
    SECTION("Valid")
    {
        auto view = PhoneNumber::validateNumber(pl_e164, pl_entered);
        REQUIRE(view.isValid());
        REQUIRE(view.getFormatted() == pl_formatted);
    }

    SECTION("Invalid")
    {
        auto view = PhoneNumber::validateNumber("+44600123456", pl_entered);
        REQUIRE_FALSE(view.isValid());
    }
}
