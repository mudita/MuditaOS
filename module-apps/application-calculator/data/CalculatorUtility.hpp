// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>

namespace CalculatorConstants
{
    inline constexpr auto veryLowPrecision = 4;
    inline constexpr auto lowPrecision     = 5;
    inline constexpr auto precision        = 6;
    inline constexpr auto highPrecision    = 8;
    inline constexpr auto expLength        = 1;
    inline constexpr auto minusExpLength   = 2;
    inline constexpr auto maxStringLength  = 7;
} // namespace CalculatorConstants

struct Result
{
    std::string equation;
    std::string value;
    bool isError;
};

class Calculator
{
    /// @param from: string which is going to be replaced
    /// @param to: string which is going to replace the old one
    /// @return: new string with replaced all occurrences of given string to the new one in whole input
    std::string replaceAllOccurrences(std::string input, const std::string &from, const std::string &to);
    std::string prepareEquationForParser(std::string input);
    std::string getValueThatFitsOnScreen(double result);
    long double getCoefficient(double result, uint32_t precision);
    std::string convertToNumberWithPositiveExponent(double result, uint32_t exponent);
    std::string convertToNumberWithNegativeExponent(double result, long long base);

  public:
    Result calculate(std::string source);
};
