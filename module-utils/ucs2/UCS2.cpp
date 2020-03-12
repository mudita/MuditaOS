/*
 * UCS2.cpp
 *
 *  Created on: 24 wrz 2019
 *      Author: kuba
 */

#include "UCS2.hpp"
#include <cstring>
#include <cstdint>
#include <sstream>

#include <iomanip>
#include "log/log.hpp"
#include <iterator>

const uint32_t UCS2::ucs2bufferExt = 16;

UCS2::UCS2(void)
{
    sizeUsed      = 0;
    sizeAllocated = ucs2bufferExt;
    buffer        = new uint16_t[ucs2bufferExt];
    length        = 0;
}

UCS2::~UCS2()
{

    delete[] buffer;
}

UCS2::UCS2(const UTF8 &string)
{
    this->clear();

    for (uint32_t i = 0; i < string.length(); i++) {
        uint32_t decodedSize;
        uint32_t utfChar = string[i];

        //		LOG_INFO("decoded uft %x", ucs2char);
        if (0xffff0000 & utfChar) {
            sizeUsed = 0;
            length   = 0;
            LOG_ERROR("UCS2::UCS2(const UTF8& string) failed, provided char is out of range");
            break;
        }

        append(static_cast<uint16_t>(utfChar));
    }
}

UCS2::UCS2(const std::string &string)
{
    sizeUsed      = 0;
    sizeAllocated = ucs2bufferExt;
    buffer        = new uint16_t[ucs2bufferExt];
    length        = 0;

    for (unsigned int i = 0; i < string.size() / 4; i++) {
        uint16_t ucs2char;
        try {
            ucs2char = std::stoi(string.substr(i * 4, 4), 0, 16);
        }
        catch (std::invalid_argument &e) {
            LOG_ERROR("UCS2::UCS2(const std::string& string) failed.");
            this->clear();
        }
        catch (std::out_of_range &e) {
            this->clear();
            LOG_ERROR("UCS2::UCS2(const std::string& string) failed.");
        }
        if (0xffff0000 & ucs2char) {
            this->clear();
            LOG_ERROR("UCS2::UCS2(const UTF8& string) failed, provided char is out of range");
            // break;
        }
        append(static_cast<uint16_t>(ucs2char));
    }

    // terminate ucs2 string by 0
    append(0);
}
UCS2::UCS2(UCS2 &ucs)
{
    sizeUsed      = 0;
    sizeAllocated = ucs2bufferExt;
    buffer        = new uint16_t[ucs2bufferExt];
    length        = 0;

    this->sizeUsed      = ucs.getSizeUsed();
    this->sizeAllocated = ucs.getSizeAlocated();
    this->length        = ucs.getLength();
    this->buffer        = new uint16_t[sizeAllocated];
    memset(buffer, 0, sizeAllocated);
    memcpy(buffer, ucs.getData(), sizeUsed);
}

UTF8 UCS2::toUTF8(void)
{
    if (this->length == 0)
        return UTF8();

    // create buffer for worst case scenario which is that every char will take 3 bytes in utf8 string
    // + 1 for null terminator

    uint8_t *buffer = new uint8_t[3 * this->length + 1];
    memset(buffer, 0, 3 * this->length + 1);

    uint32_t offset = 0;
    for (uint32_t i = 0; i < this->length; i++) {
        uint32_t c = this->buffer[i];

        // check if character must occupy 3 bytes
        if (c > 0x07ff) {
            buffer[offset++] = (0x00E0 | ((c & 0xF000) >> 12));
            buffer[offset++] = (0x0080 | ((c & 0x0FC0) >> 6));
            buffer[offset++] = (0x0080 | (c & 0x003F));
        }
        // check if character must occupy 2 bytes
        else if (c > 0x07f) {
            buffer[offset++] = (0x00C0 | ((c & 0x07C0) >> 6));
            buffer[offset++] = (0x0080 | (c & 0x003F));
        }
        else {
            buffer[offset++] = c;
        }
    }
    UTF8 retString(reinterpret_cast<const char *>(buffer));
    delete[] buffer;
    return retString;
}

void UCS2::append(const uint16_t &ucs2char)
{
    // check if buffer needs to be expanded
    if (sizeUsed == sizeAllocated) {
        uint16_t *newBuffer = new uint16_t[sizeAllocated + ucs2bufferExt];
        memset(newBuffer, 0, sizeAllocated + ucs2bufferExt);
        memcpy(newBuffer, buffer, sizeAllocated);
        delete[] buffer;
        buffer        = newBuffer;
        sizeAllocated = sizeAllocated + ucs2bufferExt;
    }
    // write character to the end of buffer, increment size and add 2 to used bytes ( usc2 character is two byte )
    buffer[length] = ucs2char;
    length++;
    sizeUsed += 2;
}

std::string UCS2::modemStr(void)
{
    std::stringstream ss;

    for (uint32_t i = 0; i < length; i++) {
        ss << std::setw(4) << std::setfill('0') << std::hex << std::uppercase;
        ss << buffer[i];
    }

    return ss.str();
}

void UCS2::clear(void)
{
    sizeUsed      = 0;
    sizeAllocated = ucs2bufferExt;

    delete[] buffer;

    buffer = new uint16_t[ucs2bufferExt];
    length = 0;
}
