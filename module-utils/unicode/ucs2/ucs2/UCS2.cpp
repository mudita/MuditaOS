// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UCS2.hpp"
#include <cstring>
#include <cstdint>
#include <sstream>

#include <iomanip>
#include <log/log.hpp>
#include <iterator>
#include <locale>
#include <codecvt>
#include <array>

namespace ucs2
{
    constexpr uint32_t bufferExt = 32;
} // namespace ucs2

UCS2::UCS2() : sizeAllocated{ucs2::bufferExt}, buffer{std::make_unique<uint32_t[]>(ucs2::bufferExt)}
{}

UCS2::UCS2(const UTF8 &string)
{
    clear();

    for (std::size_t i = 0; i < string.length(); i++) {
        uint32_t utfChar = convertFromUtf(string[i]);
        append(utfChar);
    }
}

UCS2::UCS2(const std::string &string)
    : sizeAllocated{ucs2::bufferExt}, buffer{std::make_unique<uint32_t[]>(ucs2::bufferExt)}
{
    constexpr uint8_t chunkSize16Bit = 4;
    for (std::size_t i = 0; i < string.length() / chunkSize16Bit; i++) {
        const auto ucs2char = getUcs2Char(string, i);
        // handle 32 bit
        if (0xffff0000 & ucs2char) {
            ++i;
            append(ucs2char);
            continue;
        }
        // handle 16 bit
        append(ucs2char);
    }

    // terminate ucs2 string by 0
    append(0);
}
UCS2::UCS2(UCS2 &ucs)
    : length{ucs.getLength()}, sizeUsed{ucs.getSizeUsed()},
      sizeAllocated{ucs.getSizeAlocated()}, buffer{std::make_unique<uint32_t[]>(sizeAllocated)}
{
    memcpy(buffer.get(), ucs.buffer.get(), sizeUsed);
}

UTF8 UCS2::toUTF8() const noexcept
{
    if (length == 0) {
        return UTF8();
    }

    constexpr auto maxDecodedCharSize = 3;
    std::array<uint8_t, maxDecodedCharSize> localBuffer;
    std::string decodeResult;
    decodeResult.reserve(maxDecodedCharSize * length + 1);
    for (uint32_t i = 0; i < length; i++) {
        uint32_t offset  = 0;
        const uint32_t c = this->buffer[i];
        if (c > 0xffff) {
            // 32 bit conversion
            // U' = yyyyyyyyyyxxxxxxxxxx  // U - 0x10000
            // W1 = 110110yyyyyyyyyy      // 0xD800 + yyyyyyyyyy
            // W2 = 110111xxxxxxxxxx      // 0xDC00 + xxxxxxxxxx
            const uint16_t y       = (c & 0x03FF0000) >> 16;
            const uint16_t x       = c & 0x03FF;
            const uint32_t decoded = 0x10000 + (y << 10) + x;
            decodeResult.append(convertToUtf8String({decoded}));
            continue;
        }
        else if (c > 0x07ff) {
            localBuffer[offset++] = (0x00E0 | ((c & 0xF000) >> 12));
            localBuffer[offset++] = (0x0080 | ((c & 0x0FC0) >> 6));
            localBuffer[offset++] = (0x0080 | (c & 0x003F));
        }
        else if (c > 0x07f) {
            localBuffer[offset++] = (0x00C0 | ((c & 0x07C0) >> 6));
            localBuffer[offset++] = (0x0080 | (c & 0x003F));
        }
        else {
            localBuffer[offset++] = c;
        }
        decodeResult.append(std::begin(localBuffer), std::next(std::begin(localBuffer), offset));
    }
    return UTF8(decodeResult.c_str());
}

void UCS2::append(const uint32_t &ucs2char)
{
    // check if buffer needs to be expanded
    if (sizeUsed == sizeAllocated) {
        auto newBuffer = std::make_unique<uint32_t[]>(sizeAllocated + ucs2::bufferExt);
        memcpy(newBuffer.get(), buffer.get(), sizeAllocated);
        buffer        = std::move(newBuffer);
        sizeAllocated = sizeAllocated + ucs2::bufferExt;
    }
    // write character to the end of buffer, increment size and add 2 to used bytes ( usc2 character is two byte )
    buffer[length] = ucs2char;
    length++;
    sizeUsed += sizeof(ucs2char);
}

std::string UCS2::str() const noexcept
{
    std::stringstream ss;

    for (uint32_t i = 0; i < length; i++) {
        ss << std::setw(4) << std::setfill('0') << std::hex << std::uppercase;
        ss << buffer[i];
    }

    return ss.str();
}

void UCS2::clear()
{
    sizeUsed      = 0;
    sizeAllocated = ucs2::bufferExt;

    buffer = std::make_unique<uint32_t[]>(ucs2::bufferExt);
    length = 0;
}

uint32_t UCS2::convertFromUtf(uint32_t utfChar) const noexcept
{
    if (0xffff0000 & utfChar) {
        // 32 bit conversion
        // U' = yyyyyyyyyyxxxxxxxxxx  // U - 0x10000
        // W1 = 110110yyyyyyyyyy      // 0xD800 + yyyyyyyyyy
        // W2 = 110111xxxxxxxxxx      // 0xDC00 + xxxxxxxxxx
        const uint16_t x    = utfChar & 0x3FF;
        const uint16_t y    = utfChar & 0xF400;
        const uint16_t low  = 0xDC00 + x;
        const uint16_t high = 0xD800 + (y >> 10);
        return (high << 16) + low;
    }
    return utfChar;
}

std::string inline UCS2::convertToUtf8String(const std::u32string &s) const
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(s);
}

uint32_t UCS2::getUcs2Char(const std::string &string, const std::size_t &i)
{
    constexpr uint8_t chunkSize16Bit = 4;
    constexpr uint8_t chunkSize32Bit = 8;
    constexpr uint8_t indexPosition  = 4;
    constexpr int base               = 16;

    uint32_t ucs2char     = 0;
    uint16_t nextUcs2char = 0;
    try {
        ucs2char = std::stoi(string.substr(i * indexPosition, chunkSize16Bit), 0, base);
        // check next character
        if (i < (string.length() / chunkSize16Bit) - 1) {
            nextUcs2char = std::stoi(string.substr((i + 1) * indexPosition, chunkSize16Bit), 0, base);
            // 32 bit
            if (ucs2char & 0xD800 && nextUcs2char & 0xDC00) {
                std::istringstream buf{string.substr(i * indexPosition, chunkSize32Bit)};
                buf >> std::hex >> ucs2char;
            }
        }
    }
    catch (const std::invalid_argument &e) {
        clear();
        LOG_ERROR("UCS2::UCS2(const std::string& string) failed. Invalid argument.");
    }
    catch (const std::out_of_range &e) {
        clear();
        LOG_ERROR("UCS2::UCS2(const std::string& string) failed. Out of range.");
    }

    return ucs2char;
}
