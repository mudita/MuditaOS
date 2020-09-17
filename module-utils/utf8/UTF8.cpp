#include <cassert>
#include <cstring>
#include <cstdint>
#include <ostream>

#include "UTF8.hpp"
#include "log/log.hpp"

#define debug_utf(...)

static char UTF8_EXT           = 0x80; // 1000 0000
static char UTF8_EXT_MASK      = 0xC0; // 1100 0000
static char UTF8_HEADER_1      = 0x00; // 0000 0000
static char UTF8_HEADER_1_MASK = 0x80; // 1000 0000
static char UTF8_HEADER_2      = 0xC0; // 1100 0000
static char UTF8_HEADER_2_MASK = 0xE0; // 1110 0000
static char UTF8_HEADER_3      = 0xE0; // 1110 0000
static char UTF8_HEADER_3_MASK = 0xF0; // 1111 0000
static char UTF8_HEADER_4      = 0xF0; // 1111 0000
static char UTF8_HEADER_4_MASK = 0xF8; // 1111 1000

const uint32_t UTF8::npos = uint32_t(-1);

static bool UTF8_CHAR_IS_1BYTE(const char *pc)
{
    return ((pc[0] & UTF8_HEADER_1_MASK) == UTF8_HEADER_1);
}

static bool UTF8_CHAR_IS_INNER(const char &c)
{
    return ((c & UTF8_EXT_MASK) == UTF8_EXT);
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

U8char::U8char(uint32_t code)
{
    uint32_t val = 0;
    UTF8::encode(code, val, size);

    union
    {
        char ch[utf8_max_size];
        uint32_t spread;
    } tmp{.spread = val};

    for (unsigned int i = 0; i < size; ++i) {
        utf8[size - i - 1] = tmp.ch[i];
    }
}

U8char::U8char(char *val, unsigned int size)
{
    set(val, size);
}

U8char::U8char(char *ptr)
{
    set(ptr, charLength(ptr));
}

void U8char::set(char *val, unsigned int size)
{
    assert(size < utf8_max_size);
    this->size = size;
    for (unsigned int i = 0; i < size; ++i) {
        utf8[i] = val[i];
    }
}

UTF8::UTF8()
    : data{new char[stringExpansion]}, sizeAllocated{stringExpansion}, sizeUsed{1}, strLength{0}, lastIndex{0},
      lastIndexData{data}
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
    data          = new char[sizeAllocated];
    if (data != nullptr) {
        memset(data, 0, sizeAllocated);
        memcpy(data, str, sizeUsed);
        lastIndexData = data;
    }
    strLength = getCharactersCount(data);
    lastIndex = 0;
}

UTF8::UTF8(const std::string &str)
{
    // bufferSize increased by 1 to ensure ending 0 in new string
    sizeUsed      = str.length() + 1;
    sizeAllocated = getDataBufferSize(sizeUsed);
    data          = new char[sizeAllocated];
    if (data != nullptr) {
        memset(data, 0, sizeAllocated);
        memcpy(data, str.c_str(), sizeUsed);
        lastIndexData = data;
    }
    strLength = getCharactersCount(data);
    lastIndex = 0;
}

UTF8::UTF8(const UTF8 &utf)
{
    strLength     = utf.strLength;
    sizeAllocated = utf.sizeAllocated;
    sizeUsed      = utf.sizeUsed;

    // if there is any data used in the string allocate memory and copy usedSize bytes
    if (strLength != 0) {
        data = new char[sizeAllocated];
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
        data          = new char[sizeAllocated];
        memset(data, 0, sizeAllocated);
        sizeUsed = 1;
    }
    lastIndex     = 0;
    lastIndexData = data;
}

UTF8::UTF8(UTF8 &&utf)
    : data{utf.data}, sizeAllocated{utf.sizeAllocated}, sizeUsed{utf.sizeUsed}, strLength{utf.strLength}, lastIndex{0},
      lastIndexData{data}
{
    utf.data = nullptr;
}

UTF8::UTF8(const char *data, const uint32_t allocated, const uint32_t used, const uint32_t len)
    : sizeAllocated{allocated}, sizeUsed{used}, strLength{len}, lastIndex{0}
{
    this->data = new char[allocated];
    if (this->data == nullptr) {
        sizeAllocated = 0;
        sizeUsed      = 0;
        strLength     = 0;
        return;
    }
    memcpy(this->data, data, allocated);
    lastIndexData = this->data;
}

UTF8::~UTF8()
{
    delete[] data;
}

bool UTF8::expand(uint32_t size)
{

    uint32_t newSizeAllocated = getDataBufferSize(sizeAllocated + size);
    auto *newData             = new char[newSizeAllocated];

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

    delete[] data;

    data = new char[sizeAllocated];
    memcpy(data, utf.data, sizeAllocated);

    lastIndex     = 0;
    lastIndexData = data;

    return *this;
}

UTF8 &UTF8::operator=(UTF8 &&utf) noexcept
{
    // prevent moving if object is moved to itself
    if (this != &utf) {
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

    if (idx >= strLength) {
        return 0;
    }

    char *dataPtr    = nullptr;
    uint32_t charCnt = 0;

    if (lastIndex < idx) {
        dataPtr = lastIndexData;
        charCnt = lastIndex;
    }
    else {
        dataPtr = data;
        charCnt = 0;
    }
    assert(dataPtr);
    while (charCnt != idx) {
        dataPtr += charLength(dataPtr);
        charCnt++;
    }

    lastIndex     = charCnt;
    lastIndexData = dataPtr;

    uint32_t length;
    return decode(dataPtr, length);
}

U8char UTF8::getChar(unsigned int pos)
{
    auto ptr    = data;
    long int to = pos;

    U8char u;
    while (to >= 0) {
        u = U8char(ptr);
        ptr += u.size;
        --to;
    }

    return u;
}

UTF8 UTF8::operator+(const UTF8 &utf) const
{
    // this will call copy constructor and operator += on created copy.
    return UTF8(*this) += utf;
}

UTF8 &UTF8::operator+=(const UTF8 &utf)
{
    if (utf.strLength == 0) {
        return *this;
    }

    uint32_t newSizeAllocated = getDataBufferSize(sizeUsed + utf.sizeUsed);
    auto *newData             = new char[newSizeAllocated];
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
        return memcmp(data, utf.data, sizeUsed) == 0;
    }
    return false;
}

const char *UTF8::c_str() const
{
    return data;
}

void UTF8::clear()
{
    delete[] data;
    data          = new char[stringExpansion];
    sizeAllocated = stringExpansion;
    sizeUsed      = 1;
    strLength     = 0;
    lastIndex     = 0;
    lastIndexData = data;
}

UTF8 UTF8::substr(const uint32_t begin, const uint32_t length) const
{

    if ((static_cast<uint64_t>(begin) + length > this->length()) || (length == 0)) {
        return UTF8();
    }

    char *beginPtr = this->data;
    char *endPtr   = nullptr;

    uint32_t bufferSize = 0;
    uint32_t strCounter = 0;
    // find pointer to begin char
    while (strCounter != begin) {
        beginPtr += charLength(beginPtr);
        strCounter++;
    }
    // find pinter to end char
    endPtr = beginPtr;
    for (strCounter = 0; strCounter < length; strCounter++) {
        uint32_t charSize = 0;
        charSize          = charLength(endPtr);
        endPtr += charSize;
        bufferSize += charSize;
    }
    // copy data to buffer
    // bufferSize increased by 1 to ensure ending 0 in new string
    auto *buffer = new char[bufferSize + 1];
    memset(buffer, 0, bufferSize + 1);
    memcpy(buffer, beginPtr, bufferSize);

    UTF8 retString = UTF8(buffer);
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

    if (pos + stringCount >= this->length()) {
        return npos;
    }

    uint32_t position = 0;
    auto *dataPtr     = this->data;

    for (position = 0; position < pos; position++) {
        dataPtr += charLength(dataPtr);
    }

    for (position = pos; position < this->length(); position++) {

        if (memcmp(dataPtr, s, stringSize) == 0) {
            return position;
        }
        dataPtr += charLength(dataPtr);
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
    if (pos > this->length()) {
        return npos;
    }

    if (pos < stringCount - 1) {
        return npos;
    }

    uint32_t position          = 0;
    auto *dataPtr              = this->data;
    uint32_t lastFoundPosition = npos;

    // calculate position of last string to compare
    uint32_t positionEnd = pos - stringCount + 1;
    for (position = 0; position <= positionEnd; position++) {
        if (memcmp(dataPtr, s, stringSize) == 0) {
            lastFoundPosition = position;
        }

        dataPtr += charLength(dataPtr);
    }
    return lastFoundPosition;
}

UTF8 UTF8::split(const uint32_t &idx)
{

    if (idx >= this->length()) {
        return UTF8();
    }

    auto *dataPtr = this->data;

    // move data pointer to split index
    for (uint32_t i = 0; i < idx; i++) {
        dataPtr += charLength(dataPtr);
    }
    // create new string
    UTF8 retString(dataPtr);

    // re-create source string
    // create temp copy of string
    uint32_t tempStringSize       = dataPtr - this->data;
    uint32_t tempStringBufferSize = getDataBufferSize(tempStringSize);
    auto *tempString              = new char[tempStringBufferSize];

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

UTF8 UTF8::getLine()
{
    for (uint32_t i = 0; i < this->length(); i++) {
        uint32_t character = this->operator[](i);
        if ((character == '\r') || (character == '\n')) {
            return this->substr(0, i);
        }
    }
    return UTF8();
}

bool UTF8::removeChar(const uint32_t &pos, const uint32_t &count)
{
    if (count == 0) {
        return false;
    }

    if (static_cast<uint64_t>(pos) + count > length()) // used uint64_t value to avoid 32b rounding issue
    {
        return false;
    }

    // get pointer to begin of string to remove
    auto *beginPtr = this->data;
    for (uint32_t i = 0; i < pos; i++) {
        beginPtr += charLength(beginPtr);
    }

    // get pointer to end of string to remove
    auto *endPtr = beginPtr;
    for (uint32_t i = 0; i < count; i++) {
        endPtr += charLength(endPtr);
    }

    uint32_t bytesToRemove = endPtr - beginPtr;
    uint32_t newStringSize = this->sizeUsed - bytesToRemove;

    uint32_t tempStringBufferSize = getDataBufferSize(newStringSize);
    auto *tempString              = new char[tempStringBufferSize];
    if (tempString == nullptr) {
        return false;
    }

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
    if (((code >= 0xD800) && (code <= 0xDFFF))) {
        return false;
    }

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

bool UTF8::insert(const char *ch, const uint32_t &index)
{

    // if index is different than UTF8::npos check if its valid
    auto insertIndex = index;
    if (insertIndex != UTF8::npos) {
        if (index > strLength) {
            return false;
        }
    }
    else {
        insertIndex = strLength;
    }

    // get length of the char in bytes
    uint32_t ch_len = charLength(ch);

    debug_utf("insert used: %d allocated: %d char len: %d 0x%x 0x%x 0x%x 0x%x\n",
              used(),
              allocated(),
              ch_len,
              *ch,
              *(ch + 1),
              *(ch + 2),
              *(ch + 3));

    if (ch_len == 0) {
        LOG_FATAL("not UTF8 character insert failed");
        return false;
    }

    // if there is not enough space in string buffer try to expand it by default expansion size.
    if (ch_len + sizeUsed >= sizeAllocated) {
        if (expand() == false) {
            LOG_FATAL("expand");
            return false;
        }
    }

    // find pointer where new character should be copied
    auto *pos = data;
    for (unsigned int i = 0; i < insertIndex; i++) {
        pos += charLength(pos);
    }

    if ((pos - data) >= static_cast<int64_t>(sizeUsed)) {
        debug_utf("decode/encode error %d - ( %d ) < 0 && allocated: %d\n", sizeUsed, pos - data, sizeAllocated);
        return false;
    }
    if (insertIndex != length()) {
        memmove(pos + ch_len, pos, sizeUsed - (pos - data)); // move data when insert is in text, not at the end
    }
    memcpy(pos, ch, ch_len); // copy UTF8 char value

    sizeUsed += ch_len;
    ++strLength;

    return true;
}

bool UTF8::insertCode(const uint32_t &charCode, const uint32_t &index)
{
    auto u = U8char(charCode);
    if (u.size == 0) {
        LOG_ERROR("Failed to encode value: %x", static_cast<int>(charCode));
        return false;
    }
    debug_utf("from 0x%x to size: %d -- 0x%x 0x%x\n", charCode, u.size, u.utf8[0], u.utf8[1]);
    return insert((char *)u.utf8, index);
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
    else {
        insertIndex = strLength;
    }

    uint32_t totalSize = sizeUsed + str.sizeUsed - 1; //-1 because there are 2 end terminators
    expand(getDataBufferSize(totalSize));

    auto *beginPtr = this->data;
    for (uint32_t i = 0; i < insertIndex; i++) {
        beginPtr += charLength(beginPtr);
    }

    //-1 to ignore end terminator from str
    memmove(beginPtr + str.sizeUsed - 1, beginPtr, sizeUsed - (beginPtr - data));
    memcpy(beginPtr, str.data, str.sizeUsed - 1);

    return false;
}

uint32_t UTF8::getCharactersCount(const char *stream)
{
    uint32_t size = 0, count = 0;
    if (getStreamLength(stream, size, count)) {
        return count;
    }
    return 0;
}

uint32_t UTF8::decode(const char *utf8_char, uint32_t &length)
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
        else {
            goto wrong_utf8_character;
        }
    }
    else if (((*utf8_char) & UTF8_HEADER_3_MASK) ==
             UTF8_HEADER_3) // characters number is written on 3 bytes. 1110xxxx 10xxxxxx 10xxxxxx
    {
        if (((*(utf8_char + 1) & UTF8_EXT_MASK) == UTF8_EXT) && ((*(utf8_char + 2) & UTF8_EXT_MASK) == UTF8_EXT)) {
            ret = (*utf8_char) & 0x0F;
            ret <<= 6;
            ret += *(utf8_char + 1) & 0x3F;
            ret <<= 6;
            ret += *(utf8_char + 2) & 0x3F;
            len = 3;
        }
        else {
            goto wrong_utf8_character;
        }
    }
    else if (((*(utf8_char)&UTF8_HEADER_4_MASK) ==
              UTF8_HEADER_4)) // characters number is written on 4 bytes. 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    {
        if (((*(utf8_char + 1) & UTF8_EXT_MASK) == UTF8_EXT) && ((*(utf8_char + 2) & UTF8_EXT_MASK) == UTF8_EXT) &&
            ((*(utf8_char + 3) & UTF8_EXT_MASK) == UTF8_EXT)) {
            ret = (*utf8_char) & 0x07;
            ret <<= 6;
            ret += *(utf8_char + 1) & 0x3F;
            ret <<= 6;
            ret += *(utf8_char + 2) & 0x3F;
            ret <<= 6;
            ret += *(utf8_char + 3) & 0x3F;
            len = 4;
        }
        else {
            goto wrong_utf8_character;
        }
    }
    length = len;
    return ret;
wrong_utf8_character:
    LOG_ERROR("wrong utf8 char");
    return ret;
}

std::ostream &operator<<(std::ostream &os, const UTF8 &el)
{
    os << el.c_str();
    return os;
}
