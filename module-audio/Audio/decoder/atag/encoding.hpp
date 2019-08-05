#ifndef ATAG_ENCODING_HEADER
#define ATAG_ENCODING_HEADER

#include <string>
#include <codecvt>
#include <locale>

namespace atag {
namespace encoding {

std::string utf16le_to_utf8(const char16_t* src, const int size)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t,
        0x10ffff, std::little_endian>, char16_t> convert;
    return convert.to_bytes(src, src + size);
}

std::string utf16le_to_utf8(const std::u16string& src)
{
    return utf16le_to_utf8(src.data(), src.length());
}

std::string utf16be_to_utf8(const char16_t* src, const int size)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(src, src + size);
}

std::string utf16be_to_utf8(const std::u16string& src)
{
    return utf16be_to_utf8(src.data(), src.length());
}

std::u16string utf8_to_utf16le(const char* src, const int length)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t,
        0x10ffff, std::little_endian>, char16_t> convert;
    return convert.from_bytes(src, src + length);
}

std::u16string utf8_to_utf16le(const std::string& src)
{
    return utf8_to_utf16le(src.data(), src.length());
}

std::u16string utf8_to_utf16be(const char* src, const int length)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.from_bytes(src, src + length);
}

std::u16string utf8_to_utf16be(const std::string& src)
{
    return utf8_to_utf16be(src.data(), src.length());
}

std::string iso_8859_1_to_utf8(const char* src, const int length)
{
    if(!src || (length <= 0)) { return {}; }
    std::string utf8;
    // Reserve the resulting UTF-8 string in advance to allocate only once.
    const int utf8_length = [src, length]
    {
        int size = 0;
        for(auto i = 0; (i < length) && src[i]; ++i)
        {
            if(src[i] < 128)
                ++size;
            else
                size += 2;
        }
        return size;
    }();
    utf8.reserve(utf8_length);
    // TODO should we terminate early if we encounter a null character (currently we do)
    for(auto i = 0; (i < length) && (*src != 0); ++i)
    {
        // ISO-8859-1 and UTF-8 are the same for the first 127 characters.
        if(*src < 128)
        {
            utf8.push_back(*src++);
        }
        else
        {
            utf8.push_back(192 | *src++ >> 6);
            utf8.push_back(128 | (*src++ & 63));
        }
    }
    return utf8;
}

std::string iso_8859_1_to_utf8(const std::string& src)
{
    return iso_8859_1_to_utf8(src.data(), src.length());
}

} // namespace encoding
} // namespace atag

#endif // ATAG_ENCODING_HEADER
