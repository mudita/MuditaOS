// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DTMFCode.hpp"
#include <Utils.hpp>
#include <stdexcept>

DTMFCode::DTMFCode(const char digitChar)
{
    if ((digitChar >= '0' && digitChar <= '9') || digitChar == '*' || digitChar == '#') {
        this->digitChar = digitChar;
    }
    else {
        this->digitChar = 0;
        throw std::out_of_range("DTMF: Invalid number!");
    }
}
DTMFCode::DTMFCode(const char *digitStr)
{
    auto digitTmpString = std::string{digitStr};
    if (digitTmpString == "*" || digitTmpString == "#") {
        digitChar = digitTmpString[0];
    }
    else {
        try {
            auto digit = std::stoi(digitTmpString);
            if (digit > 9) {
                throw std::out_of_range("DTMF: Invalid number!");
            }
            digitChar = digit + '0';
        }
        catch (std::invalid_argument &e) {
            LOG_ERROR("Can't parse digit string to DTMF ASCII code");
            digitChar = 0;
            throw std::out_of_range("DTMF: Invalid number!");
        }
    }
}
auto DTMFCode::getDigitASCIICode() const -> char
{
    return digitChar;
}
DTMFCode::operator std::string() const
{
    return "\"" + std::string(1, getDigitASCIICode()) + "\"";
}
