// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <data/CalculatorUtility.hpp>
#include <data/CalculatorInputProcessor.hpp>
#include <catch2/catch.hpp>
#include <i18n/i18n.hpp>
#include <cstring>

TEST_CASE("Calculator utilities")
{
    auto calculator = calc::Calculator();
    utils::setDisplayLanguage("English");

    SECTION("Empty input")
    {
        auto result = calculator.calculate("");
        REQUIRE(result.value == std::string{});
        REQUIRE(result.equation == "");
        REQUIRE(!result.isError);
    }

    SECTION("No operation")
    {
        auto result = calculator.calculate("0");
        REQUIRE(result.value == "0");
        REQUIRE(result.equation == "0");
        REQUIRE(!result.isError);

        result = calculator.calculate("1234");
        REQUIRE(result.value == "1234");
        REQUIRE(result.equation == "1234");
        REQUIRE(!result.isError);

        result = calculator.calculate("43211e6");
        REQUIRE(result.value == "4.321e10");
        REQUIRE(result.equation == "43211e6");
        REQUIRE(!result.isError);
    }

    SECTION("Addition")
    {
        auto result = calculator.calculate("5+4+8+6");
        REQUIRE(result.value == "23");
        REQUIRE(result.equation == "5+4+8+6");
        REQUIRE(!result.isError);
    }

    SECTION("Subtraction")
    {
        auto result = calculator.calculate("5-4-8-6");
        REQUIRE(result.value == "-13");
        REQUIRE(result.equation == "5-4-8-6");
        REQUIRE(!result.isError);
    }

    SECTION("Multiplication")
    {
        auto result = calculator.calculate("5×5");
        REQUIRE(result.value == "25");
        REQUIRE(result.equation == "5*5");
        REQUIRE(!result.isError);
    }

    SECTION("Division")
    {
        auto result = calculator.calculate("15÷3");
        REQUIRE(result.value == "5");
        REQUIRE(result.equation == "15/3");
        REQUIRE(!result.isError);
    }

    SECTION("Mixed operations")
    {
        auto result = calculator.calculate("15÷3+5×-5-16+40");
        REQUIRE(result.value == "4");
        REQUIRE(result.equation == "15/3+5*-5-16+40");
        REQUIRE(!result.isError);
    }

    SECTION("Fraction with full stop")
    {
        auto result = calculator.calculate("15.5+12.056");
        REQUIRE(result.value == "27.556");
        REQUIRE(result.equation == "15.5+12.056");
        REQUIRE(!result.isError);
    }

    SECTION("Fraction with comma")
    {
        utils::setDisplayLanguage("Polski");
        auto result = calculator.calculate("15,5+12,056");
        REQUIRE(result.value == "27,556");
        REQUIRE(result.equation == "15.5+12.056");
        REQUIRE(!result.isError);
    }

    SECTION("Fraction with comma - crash case [EGD-7021]")
    {
        utils::setDisplayLanguage("Polski");
        auto result = calculator.calculate("30");
        REQUIRE(result.value == "30");
        REQUIRE(result.equation == "30");
        REQUIRE(!result.isError);
    }

    SECTION("Division by 0")
    {
        auto result = calculator.calculate("15+5÷0");
        REQUIRE(result.value == calc::symbols::strings::error_str());
        REQUIRE(result.equation == "15+5/0");
        REQUIRE(result.isError);
    }

    SECTION("Division 0 by 0")
    {
        auto result = calculator.calculate("0÷0");
        REQUIRE(result.value == calc::symbols::strings::error_str());
        REQUIRE(result.equation == "0/0");
        REQUIRE(result.isError);
    }

    SECTION("Result exceeds maximum number")
    {
        auto result = calculator.calculate("1.79769e+308×2");
        REQUIRE(result.value == calc::symbols::strings::error_str());
        REQUIRE(result.equation == "1.79769e+308*2");
        REQUIRE(result.isError);
    }

    SECTION("Round to fit in screen")
    {
        auto result = calculator.calculate("1.1234512345");
        REQUIRE(result.value == "1.123451");
        REQUIRE(!result.isError);

        result = calculator.calculate("0.0567891");
        REQUIRE(result.value == "0.056789");
        REQUIRE(!result.isError);

        result = calculator.calculate("-0.056789");
        REQUIRE(result.value == "-0.05679");
        REQUIRE(!result.isError);

        result = calculator.calculate("15.556789");
        REQUIRE(result.value == "15.55679");
        REQUIRE(!result.isError);
    }

    SECTION("Change to scientific notation (number > 0)")
    {
        auto result = calculator.calculate("12345.55555");
        REQUIRE(result.value == "1.2346e4");
        REQUIRE(!result.isError);
    }

    SECTION("Change to scientific notation (number < 0)")
    {
        auto result = calculator.calculate("-12345.55555");
        REQUIRE(result.value == "-1.235e4");
        REQUIRE(!result.isError);
    }

    SECTION("Change to scientific notation (0 < number < 1)")
    {
        auto result = calculator.calculate("0.000456712");
        REQUIRE(result.value == "4.567e-4");
        REQUIRE(!result.isError);
    }

    SECTION("Change to scientific notation (-1 < number < 0)")
    {
        auto result = calculator.calculate("-0.000456712");
        REQUIRE(result.value == "-4.57e-4");
        REQUIRE(!result.isError);
    }
}
