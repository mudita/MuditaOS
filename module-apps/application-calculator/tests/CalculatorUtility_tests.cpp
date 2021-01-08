// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "application-calculator/data/CalculatorUtility.hpp"
#include <i18n/i18n.hpp>
#include <cstring>


TEST_CASE("Calculator utilities")
{
    auto calculator = Calculator();
    utils::localize.setDisplayLanguage("English");

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
        utils::localize.setDisplayLanguage("Polski");
        auto result = calculator.calculate("15,5+12,056");
        REQUIRE(result.value == "27,556");
        REQUIRE(result.equation == "15.5+12.056");
        REQUIRE(!result.isError);
    }

    SECTION("Division by 0")
    {
        auto result = calculator.calculate("15+5÷0");
        REQUIRE(result.value == utils::localize.get("app_calculator_error"));
        REQUIRE(result.equation == "15+5/0");
        REQUIRE(result.isError);
    }

    SECTION("Division 0 by 0")
    {
        auto result = calculator.calculate("0÷0");
        REQUIRE(result.value == utils::localize.get("app_calculator_error"));
        REQUIRE(result.equation == "0/0");
        REQUIRE(result.isError);
    }

    SECTION("Result exceeds maximum number")
    {
        auto result = calculator.calculate("1.79769e+308×2");
        REQUIRE(result.value == utils::localize.get("app_calculator_error"));
        REQUIRE(result.equation == "1.79769e+308*2");
        REQUIRE(result.isError);
    }
}
