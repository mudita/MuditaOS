// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalculatorInputProcessor.hpp"

bool calc::InputProcessor::isSymbol(uint32_t code) noexcept
{
    using namespace symbols::codes;

    switch (code) {
    case plus:
        [[fallthrough]];
    case minus:
        [[fallthrough]];
    case division:
        [[fallthrough]];
    case multiplication:
        [[fallthrough]];
    case comma:
        [[fallthrough]];
    case full_stop:
        return true;
    default:
        return false;
    }
}

bool calc::InputProcessor::isOperation(uint32_t code) noexcept
{
    using namespace symbols::codes;

    switch (code) {
    case plus:
        [[fallthrough]];
    case minus:
        [[fallthrough]];
    case division:
        [[fallthrough]];
    case multiplication:
        return true;
    default:
        return false;
    }
}

bool calc::InputProcessor::isDecimalSeparator(uint32_t code) noexcept
{
    using namespace symbols::codes;

    return code == comma || code == full_stop;
}
