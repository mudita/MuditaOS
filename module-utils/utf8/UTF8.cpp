/*
 * UTF8.cpp
 *
 *  Created on: 9 kwi 2019
 *      Author: robert
 */
#include <cstring>
#include <cstdint>

#include "UTF8.hpp"
#include "log/log.hpp"

static uint8_t UTF8_EXT           = 0x80; // 1000 0000
static uint8_t UTF8_EXT_MASK      = 0xC0; // 1100 0000
static uint8_t UTF8_HEADER_1      = 0x00; // 0000 0000
static uint8_t UTF8_HEADER_1_MASK = 0x80; // 1000 0000
static uint8_t UTF8_HEADER_2      = 0xC0; // 1100 0000
static uint8_t UTF8_HEADER_2_MASK = 0xE0; // 1110 0000
static uint8_t UTF8_HEADER_3      = 0xE0; // 1110 0000
static uint8_t UTF8_HEADER_3_MASK = 0xF0; // 1111 0000
static uint8_t UTF8_HEADER_4      = 0xF0; // 1111 0000
static uint8_t UTF8_HEADER_4_MASK = 0xF8; // 1111 1000

const uint32_t UTF8::npos = uint32_t(-1);

static bool UTF8_CHAR_IS_1BYTE(const char *pc)
{
    return ((pc[0] & UTF8_HEADER_1_MASK) == UTF8_HEADER_1);
}

static bool UTF8_CHAR_IS_INNER(const char &c)
{
    return ((((uint8_t)c) & UTF8_EXT_MASK) == UTF8_EXT);
}

static bool UTF8_CHAR_IS_2BYTE(const char *pc)
{
    return (((pc)[0] & UTF8_HEADER_2_MASK) == UTF8_HEADER_2) && UTF8_CHAR_IS_INNER((pc)[1]);
}

static bool UTF8_CHAR_IS_3BYTE(const char *pc)
{
    return (((pc)[0] & UTF8_HEADER_3_MASK) == UTF8_HEADER_3) && UTF8_CHAR_IS_INNER((pc)[1]) &&
           UTF8_CHAR_IS_INNER((pc)[2]);
}

static bool UTF8_CHAR_IS_4BYTE(const char *pc)
{
    return (((pc)[0] & UTF8_HEADER_4_MASK) == UTF8_HEADER_4) && UTF8_CHAR_IS_INNER((pc)[1]) &&
           UTF8_CHAR_IS_INNER((pc)[2]) && UTF8_CHAR_IS_INNER((pc)[3]);
}

static inline uint32_t charLength(const char *utf8Char)
{
    uint32_t ret = 0;
    // check if provided char is standars US-ASCII character. 0xxxxxxx
    if (UTF8_CHAR_IS_1BYTE(utf8Char)) {
        return 1;
    }
    if (UTF8_CHAR_IS_2BYTE(utf8Char)) // characters number is written on 2 bytes 110xxxxx 10xxxxxx
    {
        return 2;
    }
    if (UTF8_CHAR_IS_3BYTE(utf8Char)) // characters number is written on 3 bytes. 1110xxxx 10xxxxxx 10xxxxxx
    {
        return 3;
    }
    if (UTF8_CHAR_IS_4BYTE(utf8Char)) // characters number is written on 3 bytes. 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    {
        return 4;
    }

    return ret;
}

const char *UTF8::emptyString        = "";
const uint32_t UTF8::stringExpansion = 32;

UTF8::UTF8()
    : data{new uint8_t[stringExpansion]}, sizeAllocated{stringExpansion}, sizeUsed{1}, strLength{0}, lastIndex{0},
      lastIndexData
{
    data
}
#ifdef BUILD_GTEST_TESTS
, operator_index_iterations
{
    0
}
#endif
{
    if (data != nullptr) {
        memset(data, 0, sizeAllocated);
    }
}

UTF8::UTF8(const char *str)
{
    // bufferSize increased by 1 to ensure ending 0 in new string
    sizeUsed      = strlen(str) + 1;
    sizeAllocated = getDataBufferSize(sizeUsed);
    data          = new uint8_t[sizeAllocated];
    if (data != nullptr) {
        memset(data, 0, sizeAllocated);
        memcpy(data, str, sizeUsed);
        lastIndexData = data;
    }
    strLength = getCharactersCount(reinterpret_cast<const char *>(data));
    lastIndex = 0;
#ifdef BUILD_GTEST_TESTS
    operator_index_iterations = 0;
#endif
}

UTF8::UTF8(const std::string &str)
{
    // bufferSize increased by 1 to ensure ending 0 in new string
    sizeUsed      = str.length() + 1;
    sizeAllocated = getDataBufferSize(sizeUsed);
    data          = new uint8_t[sizeAllocated];
    if (data != nullptr) {
        memset(data, 0, sizeAllocated);
        memcpy(data, str.c_str(), sizeUsed);
        lastIndexData = data;
    }
    strLength = getCharactersCount(reinterpret_cast<const char *>(data));
    lastIndex = 0;
#ifdef BUILD_GTEST_TESTS
    operator_index_iterations = 0;
#endif
}

UTF8::UTF8(const UTF8 &utf)
{
    strLength     = utf.strLength;
    sizeAllocated = utf.sizeAllocated;
    sizeUsed      = utf.sizeUsed;

    // if there is any data used in the string allocate memory and copy usedSize bytes
    if (strLength) {
        data = new uint8_t[sizeAllocated];
        if (data == nullptr) {
            // LOG_FATAL("No memory for copy constructor.");
            sizeAllocated = 0;
            sizeUsed      = 0;
            return;
        }
        memcpy(data, utf.data, sizeAllocated);
    }
    else {
        sizeAllocated = stringExpansion;
        data          = new uint8_t[sizeAllocated];
        memset(data, 0, sizeAllocated);
        sizeUsed = 1;
    }
    lastIndex     = 0;
    lastIndexData = data;
#ifdef BUILD_GTEST_TESTS
    operator_index_iterations = 0;
#endif
}

UTF8::UTF8(UTF8 &&utf)
    : data{utf.data}, sizeAllocated{utf.sizeAllocated}, sizeUsed{utf.sizeUsed}, strLength{utf.strLength}, lastIndex{0},
      lastIndexData{data}
{
    utf.data = nullptr;
}

UTF8::UTF8(const uint8_t *data, const uint32_t allocated, const uint32_t used, const uint32_t len)
    : sizeAllocated{allocated}, sizeUsed{used}, strLength{len}, lastIndex{0}
{
    this->data = new uint8_t[allocated];
    if (this->data == nullptr) {
        sizeAllocated = 0;
        sizeUsed      = 0;
        strLength     = 0;
        return;
    }
    memcpy(this->data, data, allocated);
    lastIndexData = this->data;
#ifdef BUILD_GTEST_TESTS
    operator_index_iterations = 0;
#endif
}

UTF8::~UTF8()
{
    if (data)
        delete[] data;
}

bool UTF8::expand(uint32_t size)
{

    uint32_t newSizeAllocated = getDataBufferSize(sizeAllocated + size);
    uint8_t *newData          = new uint8_t[newSizeAllocated];

    if (newData != nullptr) {

        memcpy(newData, data, sizeUsed);
        memset(newData + sizeUsed, 0, newSizeAllocated - sizeUsed);

        delete[] data;
        data          = newData;
        sizeAllocated = newSizeAllocated;
        lastIndex     = 0;
        lastIndexData = data;
        return true;
    }
    return false;
}

uint32_t UTF8::getDataBufferSize(uint32_t dataBytes)
{
    return (((dataBytes) / stringExpansion) + 1) * stringExpansion;
}

bool UTF8::getStreamLength(const char *stream, uint32_t &size, uint32_t &count)
{
    size  = 0;
    count = 0;
    while ((*stream) != 0) {
        // calculate individual char's byte length.
        int32_t byte_length = charLength(stream);
        // if char is valid add length and increment pointer
        if (byte_length > 0) {
            size += byte_length;
            ++count;
            stream += byte_length;
        }
        else {
            size  = 0;
            count = 0;
            return false;
        }
    }
    return true;
}

UTF8 &UTF8::operator=(const UTF8 &utf)
{

    // prevent from making assignment to itself
    if (this == &utf) {
        return *this;
    }

    sizeAllocated = utf.sizeAllocated;
    sizeUsed      = utf.sizeUsed;
    strLength     = utf.strLength;

    if (data)
        delete[] data;

    data = new uint8_t[sizeAllocated];
    memcpy(data, utf.data, sizeAllocated);

    lastIndex     = 0;
    lastIndexData = data;

    return *this;
}

UTF8 &UTF8::operator=(UTF8 &&utf) noexcept
{
    // prevent moving if object is moved to itself
    if (this != &utf) {
        if (data)
            delete[] data;
        data          = utf.data;
        utf.data      = nullptr;
        sizeAllocated = utf.sizeAllocated;
        sizeUsed      = utf.sizeUsed;
        strLength     = utf.strLength;
    }
    return *this;
}

uint32_t UTF8::operator[](const uint32_t &idx) const
{

    if (idx >= strLength)
        return 0;

    uint8_t *dataPtr;
    uint32_t charCnt = 0;
    if (lastIndex < idx) {
        dataPtr = lastIndexData;
        charCnt = lastIndex;
    }
    else {
        dataPtr = data;
        charCnt = 0;
    }
#ifdef BUILD_GTEST_TESTS
    operator_index_iterations = 0;
#endif
    while (charCnt != idx) {
        dataPtr += charLength(reinterpret_cast<const char *>(dataPtr));
        charCnt++;
#ifdef BUILD_GTEST_TESTS
        operator_index_iterations++;
#endif
    }

    lastIndex     = charCnt;
    lastIndexData = dataPtr;

    uint32_t dummy;
    uint32_t retValue = decode(reinterpret_cast<const char *>(dataPtr), dummy);
    return retValue;
}

UTF8 UTF8::operator+(const UTF8 &utf)
{
    // this will call copy constructor and operator += on created copy.
    return UTF8(*this) += utf;
}

UTF8 &UTF8::operator+=(const UTF8 &utf)
{

    if (utf.strLength == 0)
        return *this;

    uint32_t newSizeAllocated = getDataBufferSize(sizeUsed + utf.sizeUsed);
    uint8_t *newData          = new uint8_t[newSizeAllocated];
    if (newData != nullptr) {

        memcpy(newData, data, sizeUsed);
        //-1 comes from the fact that null terminator is counted as a used byte in string's buffer.
        memcpy(newData + sizeUsed - 1, utf.data, utf.sizeUsed);
        delete[] data;
        data          = newData;
        sizeAllocated = newSizeAllocated;
        strLength += utf.strLength;
        //-1 is to ignore double null terminator as it is counted in sizeUsed
        sizeUsed += utf.sizeUsed - 1;
        lastIndex     = 0;
        lastIndexData = data;
    }
    return *this;
}

bool UTF8::operator==(const UTF8 &utf) const
{
    uint32_t len  = strLength - utf.strLength;
    uint32_t used = sizeUsed - utf.sizeUsed;
    if ((len | used) == 0) {
        return !memcmp(data, utf.data, sizeUsed);
    }
    return false;
}

const char *UTF8::c_str() const
{
    return reinterpret_cast<const char *>(data);
}

void UTF8::clear()
{

    if (data)
        delete data;

    data          = new uint8_t[stringExpansion];
    sizeAllocated = stringExpansion;
    sizeUsed      = 1;
    strLength     = 0;
    lastIndex     = 0;
    lastIndexData = data;
}

UTF8 UTF8::substr(const uint32_t begin, const uint32_t length) const
{

    if ((begin + length > this->length()) || (length == 0))
        return UTF8();

    uint8_t *beginPtr = this->data;
    uint8_t *endPtr;

    uint32_t bufferSize = 0;
    uint32_t strCounter = 0;
    // find pointer to begin char
    while (strCounter != begin) {
        beginPtr += charLength(reinterpret_cast<const char *>(beginPtr));
        strCounter++;
    }
    // find pinter to end char
    endPtr = beginPtr;
    for (strCounter = 0; strCounter < length; strCounter++) {
        uint32_t charSize = 0;
        charSize          = charLength(reinterpret_cast<const char *>(endPtr));
        endPtr += charSize;
        bufferSize += charSize;
    }
    // copy data to buffer
    // bufferSize increased by 1 to ensure ending 0 in new string
    uint8_t *buffer = new uint8_t[bufferSize + 1];
    memset(buffer, 0, bufferSize + 1);
    memcpy(buffer, beginPtr, bufferSize);

    UTF8 retString = UTF8(reinterpret_cast<const char *>(buffer));
    delete[] buffer;

    return retString;
}

uint32_t UTF8::find(const char *s, uint32_t pos)
{
    uint32_t stringCount;
    uint32_t stringSize;

    getStreamLength(s, stringSize, stringCount);

    if ((stringCount == 0) && (s[0] != 0)) {
        LOG_ERROR("corrupted string");
        return npos;
    }

    if (pos + stringCount >= this->length())
        return npos;

    uint32_t position = 0;
    uint8_t *dataPtr  = this->data;

    for (position = 0; position < pos; position++) {
        dataPtr += charLength(reinterpret_cast<const char *>(dataPtr));
    }

    for (position = pos; position < this->length(); position++) {

        if (memcmp(dataPtr, s, stringSize) == 0)
            return position;
        dataPtr += charLength(reinterpret_cast<const char *>(dataPtr));
    }
    return npos;
}

uint32_t UTF8::findLast(const char *s, uint32_t pos)
{
    uint32_t stringCount;
    uint32_t stringSize;

    getStreamLength(s, stringSize, stringCount);
    // check input substring
    if ((stringCount == 0) && (s[0] != 0)) {
        LOG_ERROR("corrupted string");
        return npos;
    }
    // check if pos is in range of source string
    if (pos > this->length())
        return npos;

    if (pos < stringCount - 1)
        return npos;

    uint32_t position         = 0;
    uint8_t *dataPtr          = this->data;
    uint32_t lastFoundPosition = npos;

    // calculate position of last string to compare
    uint32_t positionEnd = pos - stringCount + 1;
    for (position = 0; position <= positionEnd; position++) {
        if (memcmp(dataPtr, s, stringSize) == 0)
            lastFoundPosition = position;

        dataPtr += charLength(reinterpret_cast<const char *>(dataPtr));
    }
    return lastFoundPosition;
}

UTF8 UTF8::split(const uint32_t &idx)
{

    if (idx >= this->length())
        return UTF8();

    uint8_t *dataPtr = this->data;

    // move data pointer to split index
    for (uint32_t i = 0; i < idx; i++) {
        dataPtr += charLength(reinterpret_cast<const char *>(dataPtr));
    }
    // create new string
    UTF8 retString(reinterpret_cast<const char *>(dataPtr));

    // re-create source string
    // create temp copy of string
    uint32_t tempStringSize       = dataPtr - this->data;
    uint32_t tempStringBufferSize = getDataBufferSize(tempStringSize);
    uint8_t *tempString           = new uint8_t[tempStringBufferSize];

    memset(tempString, 0, tempStringBufferSize);
    memcpy(tempString, this->data, tempStringSize);

    // add 1 to ensure string terminating zero
    this->sizeUsed      = tempStringSize + 1;
    this->sizeAllocated = tempStringBufferSize;
    this->strLength     = idx;

    // clear used memory
    memset(this->data, 0, this->sizeAllocated);
    delete[] this->data;

    this->data = tempString;

    this->lastIndex     = 0;
    this->lastIndexData = this->data;

    return retString;
}

UTF8 UTF8::getLine(void)
{
    for (uint32_t i = 0; i < this->length(); i++) {
        uint32_t character = this->operator[](i);
        if ((character == '\r') || (character == '\n'))
            return this->substr(0, i);
    }
    return UTF8();
}

bool UTF8::removeChar(const uint32_t &pos, const uint32_t &count)
{

    if ((pos + count > this->length()) || (count == 0))
        return false;

    // get pointer to begin of string to remove
    uint8_t *beginPtr = this->data;
    for (uint32_t i = 0; i < pos; i++) {
        beginPtr += charLength(reinterpret_cast<const char *>(beginPtr));
    }

    // get pointer to end of string to remove
    uint8_t *endPtr = beginPtr;
    for (uint32_t i = 0; i < count; i++) {
        endPtr += charLength(reinterpret_cast<const char *>(endPtr));
    }

    uint32_t bytesToRemove = endPtr - beginPtr;
    uint32_t newStringSize = this->sizeUsed - bytesToRemove;

    uint32_t tempStringBufferSize = getDataBufferSize(newStringSize);
    uint8_t *tempString           = new uint8_t[tempStringBufferSize];
    if (tempString == nullptr)
        return false;

    // create new data buffer
    uint32_t copyOffset = beginPtr - this->data;
    memset(tempString, 0, tempStringBufferSize);
    memcpy(tempString, this->data, beginPtr - this->data);
    memcpy(tempString + copyOffset, endPtr, this->sizeUsed - bytesToRemove - copyOffset);

    this->sizeAllocated = tempStringBufferSize;
    this->strLength -= count;
    this->sizeUsed -= bytesToRemove;

    // delete old data buffer
    memset(this->data, 0, this->sizeAllocated);
    delete[] this->data;

    // assign new data buffer
    this->data = tempString;

    return true;
}

// Char. number range  |        UTF-8 octet sequence
//   (hexadecimal)    |              (binary)
//--------------------+---------------------------------------------
// 0000 0000-0000 007F | 0xxxxxxx
// 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
// 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
// 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
bool UTF8::encode(const uint16_t &code, uint32_t &dest, uint32_t &length)
{

    dest   = 0;
    length = 0;
    if (((code >= 0xD800) && (code <= 0xDFFF)))
        return false;

    if (code < 0x00080) {
        length = 1;
        dest   = code;
    }
    else if (code < 0x0800) {
        length = 2;
        dest   = 0x0000C080;
        // low byte
        dest |= (code & 0x003F);
        // high byte
        dest |= ((code >> 6) & 0x001F) << 8;
    }
    else {
        length = 4;
        dest   = 0xF0808080;
        // low byte
        dest |= (code & 0x003F);
        // byte after low
        dest |= ((code >> 6) & 0x003F) << 8;
        // byte before low
        dest |= ((code >> 12) & 0x003F) << 16;
        // high byte
        dest |= ((code >> 18) & 0x0007) << 24;
    }
    return true;
}

bool UTF8::insert(const char *charPtr, const uint32_t &index)
{

    // if index is different than UTF8::npos check if its valid
    auto insertIndex = index;
    if (insertIndex != UTF8::npos) {
        if (index > strLength) {
            return false;
        }
    }
    else
        insertIndex = strLength;

    // get length of the char in bytes
    uint32_t charLen = charLength(charPtr);

    // if there is not enough space in string buffer try t expand it by default expansion size.
    if (charLen + sizeUsed >= sizeAllocated) {
        if (expand() == false)
            return false;
    }

    // find pointer where new character should be copied
    uint8_t *beginPtr = this->data;
    for (unsigned int i = 0; i < insertIndex; i++) {
        beginPtr += charLength(reinterpret_cast<const char *>(beginPtr));
    }

    memmove(beginPtr + charLen, beginPtr, sizeUsed - (beginPtr - data));
    memcpy(beginPtr, charPtr, charLen);

    sizeUsed += charLen;
    ++strLength;

    return true;
}

bool UTF8::insertCode(const uint32_t &charCode, const uint32_t &index)
{

    uint32_t encodedValue;
    uint32_t encodedLength;
    encode(charCode, encodedValue, encodedLength);

    return insert(reinterpret_cast<char *>(&encodedValue), index);
}

bool UTF8::insertString(const UTF8 &str, const uint32_t &index)
{

    // if index is different than UTF8::npos check if its valid
    auto insertIndex = index;
    if (insertIndex != UTF8::npos) {
        if (index > strLength) {
            return false;
        }
    }
    else
        insertIndex = strLength;

    uint32_t totalSize = sizeUsed + str.sizeUsed - 1; //-1 because there are 2 end terminators
    expand(getDataBufferSize(totalSize));

    uint8_t *beginPtr = this->data;
    for (uint32_t i = 0; i < insertIndex; i++) {
        beginPtr += charLength(reinterpret_cast<const char *>(beginPtr));
    }

    //-1 to ignore end terminator from str
    memmove(beginPtr + str.sizeUsed - 1, beginPtr, sizeUsed - (beginPtr - data));
    memcpy(beginPtr, str.data, str.sizeUsed - 1);

    return false;
}

uint32_t UTF8::getCharactersCount(const char *stream)
{
    uint32_t size, count;
    if (getStreamLength(stream, size, count))
        return count;
    return 0;
}

uint8_t *UTF8::serialize(uint32_t &streamSize)
{

    streamSize = 0;
    // size of the buffer
    uint32_t size = sizeAllocated + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
    // buffer for storing data
    uint8_t *buf = (uint8_t *)malloc(size);

    if (buf == NULL) {
        // LOG_FATAL("No memory to serialize string");
        return NULL;
    }
    memcpy(buf, &sizeAllocated, sizeof(uint32_t));
    memcpy(buf + sizeof(uint32_t), &sizeUsed, sizeof(uint32_t));
    memcpy(buf + sizeof(uint32_t) + sizeof(uint32_t), &strLength, sizeof(uint32_t));
    memcpy(buf + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), data, sizeAllocated);

    streamSize = size;

    return buf;
}

UTF8 UTF8::deserialize(uint8_t *stream)
{

    if (stream == nullptr) {
        // LOG_ERROR("null stream provided.");
        return UTF8();
    }
    uint32_t *sizeAllocated = reinterpret_cast<uint32_t *>(stream);
    uint32_t *sizeUsed      = reinterpret_cast<uint32_t *>(stream + sizeof(uint32_t));
    uint32_t *strLength     = reinterpret_cast<uint32_t *>(stream + 2 * sizeof(uint32_t));
    uint8_t *data           = stream + 3 * sizeof(uint32_t);

    // sanity check
    if (((*strLength) > (*sizeUsed)) || ((*sizeUsed) > (*sizeAllocated))) {
        // LOG_FATAL("Errors in stream detected.");
        return UTF8();
    }
    return UTF8(data, *sizeAllocated, *sizeUsed, *strLength);
}

uint32_t UTF8::decode(const char *utf8_char, uint32_t &length) const
{
    uint32_t ret = 0;
    uint32_t len = 0;
    // check if provided char is standards US-ASCII character. 0xxxxxxx
    if (((*utf8_char) & UTF8_HEADER_1_MASK) == 0) {
        ret = *utf8_char;
        len = 1;
    }
    else if (((*utf8_char) & UTF8_HEADER_2_MASK) ==
             UTF8_HEADER_2) // characters number is written on 2 bytes 110xxxxx 10xxxxxx
    {
        if ((*(utf8_char + 1) & UTF8_EXT_MASK) == UTF8_EXT) {
            ret = (*utf8_char) & 0x1F;
            ret <<= 6;
            ret += *(utf8_char + 1) & 0x3F;
            len = 2;
        }
        else
            goto wrong_utf8_character;
    }
    else if (((*utf8_char) & UTF8_HEADER_3_MASK) ==
             UTF8_HEADER_3) // characters number is written on 3 bytes. 1110xxxx 10xxxxxx 10xxxxxx
    {
        if (((*(utf8_char + 1) & UTF8_EXT_MASK) == UTF8_EXT) && ((*(utf8_char + 2) & UTF8_EXT_MASK) == UTF8_EXT)) {
            ret = (*utf8_char) & 0x7F;
            ret <<= 6;
            ret += *(utf8_char + 1) & 0x3F;
            ret <<= 6;
            ret += *(utf8_char + 2) & 0x3F;
            len = 3;
        }
        else
            goto wrong_utf8_character;
    }
    else if (((*(utf8_char)&UTF8_HEADER_4_MASK) ==
              UTF8_HEADER_4)) // characters number is written on 3 bytes. 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    {
        if (((*(utf8_char + 1) & UTF8_EXT_MASK) == UTF8_EXT) && ((*(utf8_char + 2) & UTF8_EXT_MASK) == UTF8_EXT) &&
            ((*(utf8_char + 3) & UTF8_EXT_MASK) == UTF8_EXT)) {
            ret = (*utf8_char) & 0x7F;
            ret <<= 6;
            ret += *(utf8_char + 1) & 0x3F;
            ret <<= 6;
            ret += *(utf8_char + 2) & 0x3F;
            ret <<= 6;
            ret += *(utf8_char + 3) & 0x3F;
            len = 4;
        }
        else
            goto wrong_utf8_character;
    }
    length = len;
    return ret;
wrong_utf8_character:
    return ret;
}
