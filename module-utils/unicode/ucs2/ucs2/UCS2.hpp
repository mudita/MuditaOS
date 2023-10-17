// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <memory>
#include <utf8/UTF8.hpp>

class UCS2
{
  private:
    // number of characters in the string. its equal to size of allocated memory plus null terminator
    uint32_t length = 0U;
    // size in bytes of memory used in buffer
    uint32_t sizeUsed = 0U;
    // size in bytes of memory that was allcated to the buffer
    uint32_t sizeAllocated = 0U;
    // pointer to memory where ucs2 characters are stored.
    std::unique_ptr<uint32_t[]> buffer;
    void clear();
    [[nodiscard]] uint32_t convertFromUtf(uint32_t utfChar) const noexcept;

    [[nodiscard]] std::string inline convertToUtf8String(const std::u32string &s) const;
    [[nodiscard]] uint32_t getUcs2Char(const std::string &string, const std::size_t &i);

  public:
    UCS2();
    /*
     * @brief Initializes new ucs2 string from utf8 string. It's used to convert text from
     * utf8 to ucs2.
     * @param string utf8 string to convert
     */
    explicit UCS2(const UTF8 &string);
    /*
     * @brief Initializes new ucs2 string from std::string. It's used to convert text from
     * modem message format to ucs2.
     * @param string std::string to convert
     */
    explicit UCS2(const std::string &string);
    explicit UCS2(UCS2 &ucs);
    [[nodiscard]] const char *c_str() const noexcept
    {
        return reinterpret_cast<const char *>(buffer.get());
    }
    /*
     * @brief It's converting ucs2 to utf string.
     * @return utf8 string
     */
    [[nodiscard]] UTF8 toUTF8() const noexcept;
    void append(const uint32_t &ucs2char);
    /*
     * @brief It's converting text coded in ucs2 to string. Used to send data to modem.
     * @return coded string
     */
    [[nodiscard]] std::string str() const noexcept;
    [[nodiscard]] uint32_t getLength() const noexcept
    {
        return length;
    };
    [[nodiscard]] uint32_t getSizeUsed() const noexcept
    {
        return sizeUsed;
    };
    [[nodiscard]] uint32_t getSizeAlocated() const noexcept
    {
        return sizeAllocated;
    };
};
