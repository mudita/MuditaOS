// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <cstdint>
#include <iosfwd> // for forward declaration for ostream
#include <memory>
#include <optional>

/// single utf8 character representation struct
struct U8char
{
    U8char() = default;
    /// get UTF16 (or U+ ) value and store it as UTF8
    /// @note this is done for little endian
    U8char(uint32_t code);
    /// get UTF8 and store it
    /// @note unsafe
    U8char(char *val, unsigned int size);
    /// get UTF8 code from char* stream depending on size of data
    /// @note unsafe
    U8char(char *);

    static const unsigned int utf8_max_size = 4;

    unsigned char utf8[utf8_max_size];

    uint32_t size = 0;

    void set(char *val, unsigned int size);
    void set(uint32_t code);
};

class UTF8
{
  protected:
    UTF8(const char *data, const uint32_t allocated, const uint32_t used, const uint32_t len);

    /// pointer to buffer
    std::unique_ptr<char[]> data;
    /// total size of buffer in bytes
    uint32_t sizeAllocated;
    /// number of bytes used in buffer
    uint32_t sizeUsed;
    /// umber of characters in the string
    uint32_t strLength;
    /// last used index
    mutable uint32_t lastIndex;
    /// pointer to last indexed character
    mutable char *lastIndexData;

    /// variable used when c_str() is called for a string that has no data yet
    static const char *emptyString;
    /// holds number of bytes by which buffer will be expanded in case when current buffer can't hold new data.
    static const uint32_t stringExpansion;
    /**
     * @brief Calculates size of the buffer to store given number of data bytes.
     * @param dataBytes number of data bytes
     * @return Number of bytes needed to store provided data bytes size rounded up and rounded using stringExpansion
     * value.
     */
    uint32_t getDataBufferSize(uint32_t dataBytes);
    bool expand(uint32_t size = stringExpansion);

  public:
    UTF8();
    UTF8(const char *str);
    UTF8(const std::string &str);
    UTF8(const UTF8 &utf);
    UTF8(UTF8 &&utf);
    static const uint32_t npos;

    virtual ~UTF8() = default;

    /**
     * OPERATORS
     */

    UTF8 &operator=(const UTF8 &utf);
    UTF8 &operator=(UTF8 &&utf) noexcept;
    /// returns UTF16 value of character (this is - utf8 value encoded to utf16)
    /// for utf8 value please use getChar
    uint32_t operator[](const uint32_t &idx) const;
    UTF8 operator+(const UTF8 &utf) const;
    UTF8 &operator+=(const UTF8 &utf);
    bool operator==(const UTF8 &utf) const;
    bool operator!=(const UTF8 &utf) const
    {
        return !operator==(utf);
    }
    operator std::string() const
    {
        return c_str();
    }
    operator std::string_view() const
    {
        return c_str();
    }
    friend std::ostream &operator<<(std::ostream &os, const UTF8 &el);

    /**
     * UTILITY FUNCTIONS
     */

    uint32_t length() const
    {
        return strLength;
    }

    bool empty() const noexcept
    {
        return strLength == 0U;
    }

    uint32_t used() const
    {
        return sizeUsed;
    }
    uint32_t allocated() const
    {
        return sizeAllocated;
    }
    const char *c_str() const;

    /// returns utf8 value on position, to get utf16 use operator[]
    U8char getChar(unsigned int pos);

    /**
     * @brief Removes all content from the string and reduce assigned memory to default value.
     */
    void clear();

    /**
     *@brief Creates substring from current string. New string starts from begin parameter and contains number of
     *characters passed by length.
     *@param begin Index of the first character in newly created string.
     *@param length Number of characters to copy.
     *@return substring created from source string.
     *@note In case of start index greater than length of source string or length that exceeds character empty string is
     *returned.
     **/
    UTF8 substr(const uint32_t begin, const uint32_t length) const;

    /**
     * @brief Finds first occurrence of substring in string
     * @param s string to find
     * @param pos initial searching position
     * @return index of first matched string
     * @note returns npos when substring is not found
     */

    uint32_t find(const char *s, uint32_t pos = 0) const;
    /**
     * @brief Finds last occurrence of substring in string
     * @param s string to find
     * @param pos initial searching position
     * @return index of first matched string
     * @note returns npos when substring is not found.
     */
    uint32_t findLast(const char *s, uint32_t pos) const;
    /**
     * @brief splits UTF8 sting into two strings.
     * @param idx index of character from which the division will be made.
     * @return newly created string, character under specified index will be first character in new string.
     * Returns empty string in case of invalid index.
     */
    UTF8 split(const uint32_t &idx);
    /**
     * @brief Creates substring from current string. New string is limited by /r or /n.
     * @return subrstring created from current string. Returns empty string in case of failure.
     */
    UTF8 getLine(void);
    /**
     * @brief Remove characters from string.
     * @param pos position of first char to remove.
     * @param count count of characters to remove.
     * @return true if there was no error, false otherwise
     */
    bool removeChar(const uint32_t &pos = 0, const uint32_t &count = 1);
    /**
     * @brief Inserts character into string on specified position. If position is not specified (UTF8::npos) char is
     * added at the end to current string.
     * @param charPtr pointer to the memory where UTF8 character is located.
     * @param index index in the current string where character should be inserted.
     * @return true is operation was successful false otherwise.
     */
    bool insert(const char *charPtr, const uint32_t &index = UTF8::npos);
    /**
     * @brief Inserts character into string on specified position. If position is not specified (UTF8::npos) char is
     * added at the end to current string.
     * @param charCode code of the character to insert.
     * @param index index in the current string where character should be inserted.
     * @return true is operation was successful false otherwise.
     */
    bool insertCode(const uint32_t &charCode, const uint32_t &index = UTF8::npos);
    /**
     * @brief Inserts string into current string on specified position. If position is not specified (UTF8::npos) is
     * appended at the end of the current string.
     * @param str String to be inserted into current object.
     * @param index index in the current string where character should be inserted.
     * @return true is operation was successful false otherwise.
     */
    bool insertString(const UTF8 &str, const uint32_t &index = UTF8::npos);

    /*
     * @brief Check if string has only ASCII characters
     * @return true if there are only ASCII characters in string, false otherwise.
     */
    bool isAscii(void) const
    {
        if (this->sizeUsed - 1 == this->length())
            return true;
        return false;
    }
    /**
     * @brief Returns pointer to character encoded using provided Unicode value.
     * @param code Unicode of the character.
     * @param dest uint32 variable where encoded variable will be stored.
     * @param length number of useful bytes in dest variable.
     * @return True if encoding was successful, false otherwise
     * @note function returns false if character is within prohibited range - <U+D800, U+DFFF> or above value of
     * U+10FFFF.
     */
    static bool encode(const uint32_t &code, uint32_t &dest, uint32_t &length);

    /// get utf16_t value from utf8 character
    static uint32_t decode(const char *utf8_char, uint32_t &length);

    /**
     * @brief Calculates numer of utf8 characters in provided stream
     */
    static uint32_t getCharactersCount(const char *stream);
    /**
     * @brief Calculates number of bytes and character IDs in the provided stream;
     * @param size Variable where number of bytes in the stream will be saved (till null terminator).
     * @param count Variable where number of characters in the stream will be saved.
     * @return true if there was no error, false otherwise.
     */
    static bool getStreamLength(const char *stream, uint32_t &size, uint32_t &count);
    /**
     * @brief Checks if numbers contained in the UTF8 creates ASCII character combination
     * eg. 778568738465 returns true
     * @return true if is acii combination, false otherwise
     */
    [[nodiscard]] bool isASCIICombination() const noexcept;
    /**
     * @brief Converts UTF8 to ASCII character combination
     * eg. 778568738465 returns "MUDITA"
     * @return ASCII converted string
     */
    [[nodiscard]] std::optional<std::string> toASCII() const noexcept;
};
