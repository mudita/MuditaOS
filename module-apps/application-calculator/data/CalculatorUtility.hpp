// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace calc
{
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

} // namespace calc
