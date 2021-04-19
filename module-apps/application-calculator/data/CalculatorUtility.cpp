// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalculatorUtility.hpp"
#include "application-calculator/data/CalculatorInputProcessor.hpp"
#include <module-utils/tinyexpr/tinyexpr.h>
#include <i18n/i18n.hpp>
#include <Utils.hpp>
#include <cmath>

namespace calc
{
    Result Calculator::calculate(std::string source)
    {
        source = prepareEquationForParser(source);

        if (source.empty()) {
            return Result{source, {}, false};
        }

        int error;
        double result = te_interp(source.c_str(), &error);
        if (error == 0 && !std::isinf(result) && !std::isnan(result)) {
            auto output = utils::to_string(result);
            if (output.length() > CalculatorConstants::maxStringLength) {
                output = getValueThatFitsOnScreen(result);
            }
            if (utils::localize.get("app_calculator_decimal_separator") == symbols::strings::comma) {
                output.replace(output.find(symbols::strings::full_stop),
                               std::size(std::string_view(symbols::strings::full_stop)),
                               symbols::strings::comma);
            }
            return Result{source, output, false};
        }
        return Result{source, utils::localize.get("app_calculator_error"), true};
    }

    std::string Calculator::prepareEquationForParser(std::string input)
    {
        input = replaceAllOccurrences(input, symbols::strings::division, symbols::strings::solidus);
        input = replaceAllOccurrences(input, symbols::strings::multiplication, symbols::strings::asterisk);
        input = replaceAllOccurrences(input, symbols::strings::comma, symbols::strings::full_stop);
        return input;
    }

    std::string Calculator::replaceAllOccurrences(std::string input, const std::string &from, const std::string &to)
    {
        size_t index = 0;
        while (true) {
            index = input.find(from, index);
            if (index == std::string::npos)
                break;
            input.replace(index, from.length(), to);
            index += to.length();
        }
        return input;
    }

    std::string Calculator::getValueThatFitsOnScreen(double result)
    {
        auto base   = static_cast<long long>(result);
        auto length = utils::to_string(base).length();
        if (base < 0) {
            length -= 1;
        }
        if (length > CalculatorConstants::expLength + 1) {
            return convertToNumberWithPositiveExponent(result, length - 1);
        }
        else if (length == CalculatorConstants::expLength + 1) {
            if (result < 0) {
                return utils::to_string(getCoefficient(result, CalculatorConstants::veryLowPrecision));
            }
            return utils::to_string(getCoefficient(result, CalculatorConstants::lowPrecision));
        }
        else if (length == 1 && result < -1) {
            return utils::to_string(getCoefficient(result, CalculatorConstants::lowPrecision));
        }
        else if (result > 1) {
            return utils::to_string(getCoefficient(result, CalculatorConstants::precision));
        }
        else {
            return convertToNumberWithNegativeExponent(result, base);
        }
    }

    std::string Calculator::convertToNumberWithPositiveExponent(double result, uint32_t exponent)
    {
        result /= pow(10, exponent);
        auto exponentLength = utils::to_string(exponent).length();
        auto decimalPlace   = CalculatorConstants::precision - exponentLength - CalculatorConstants::expLength;
        if (result < 0) {
            decimalPlace -= 1;
        }
        return utils::to_string(getCoefficient(result, decimalPlace)) + "e" + utils::to_string(exponent);
    }

    std::string Calculator::convertToNumberWithNegativeExponent(double result, long long base)
    {
        double frac = (result - base) * pow(10, CalculatorConstants::highPrecision);
        if (result < 0) {
            frac *= -1;
        }
        auto fractionalPart = static_cast<unsigned long int>(round(frac));
        auto fracLength     = utils::to_string(fractionalPart).length();
        auto exponent       = CalculatorConstants::highPrecision - fracLength + 1;
        if (exponent > CalculatorConstants::minusExpLength + 1) {
            result *= pow(10, exponent);
            auto exponentLength = utils::to_string(exponent).length();
            auto decimalPlace   = CalculatorConstants::precision - exponentLength - CalculatorConstants::minusExpLength;
            if (result < 0) {
                decimalPlace -= 1;
            }
            return utils::to_string(getCoefficient(result, decimalPlace)) + "e-" + utils::to_string(exponent);
        }
        else if (result < 0) {
            return utils::to_string(getCoefficient(result, CalculatorConstants::lowPrecision));
        }
        return utils::to_string(getCoefficient(result, CalculatorConstants::precision));
    }

    long double Calculator::getCoefficient(double result, uint32_t precision)
    {
        return std::roundl(result * pow(10, precision)) / pow(10, precision);
    }

} // namespace calc
