// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalculatorUtility.hpp"
#include "application-calculator/widgets/CalculatorStyle.hpp"
#include <module-utils/tinyexpr/tinyexpr.h>
#include <module-utils/i18n/i18n.hpp>
#include <Utils.hpp>
#include <cmath>

Result Calculator::calculate(std::string source)
{
    source = prepareEquationForParser(source);
    int error;
    double result = te_interp(source.c_str(), &error);
    if (error == 0 && !std::isinf(result) && !std::isnan(result)) {
        auto output = utils::to_string(result);
        if (utils::localize.get("app_calculator_decimal_separator") == style::calculator::symbols::strings::comma) {
            output.replace(output.find(style::calculator::symbols::strings::full_stop),
                           std::size(std::string_view(style::calculator::symbols::strings::full_stop)),
                           style::calculator::symbols::strings::comma);
        }
        return Result{source, output, false};
    }
    return Result{source, utils::localize.get("app_calculator_error"), true};
}

std::string Calculator::prepareEquationForParser(std::string input)
{
    input = replaceAllOccurrences(
        input, style::calculator::symbols::strings::division, style::calculator::symbols::strings::solidus);
    input = replaceAllOccurrences(
        input, style::calculator::symbols::strings::multiplication, style::calculator::symbols::strings::asterisk);
    input = replaceAllOccurrences(
        input, style::calculator::symbols::strings::comma, style::calculator::symbols::strings::full_stop);
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
