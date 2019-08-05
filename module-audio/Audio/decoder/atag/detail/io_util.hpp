#ifndef ATAG_IO_UTIL_HEADER
#define ATAG_IO_UTIL_HEADER

#include <cstdint>

namespace atag {
namespace detail {

template<typename InputIt>
uint32_t parse_syncsafe_int(InputIt it) noexcept
{
    uint32_t t = 0;
    t |= static_cast<uint32_t>(static_cast<uint8_t>(*it++)) << 3 * 7;
    t |= static_cast<uint32_t>(static_cast<uint8_t>(*it++)) << 2 * 7;
    t |= static_cast<uint32_t>(static_cast<uint8_t>(*it++)) <<     7;
    t |= static_cast<uint32_t>(static_cast<uint8_t>(*it++));
    return t;
}

template<typename T, typename InputIt>
T parse_syncsafe(InputIt it) noexcept
{
    T t = 0;
    for(auto i = 0; i < int(sizeof t); ++i)
    {
        t <<= 7;
        t |= static_cast<uint8_t>(*it++);
    }
    return t;
}

template<typename T, typename OutputIt>
void write_syncsafe(T t, OutputIt it) noexcept
{
    // TODO is this correct?
    for(int shift = 7 * (int(sizeof t) - 1); shift >= 0; shift -= 7)
    {
        *it++ = static_cast<uint8_t>((t >> shift) & 0xff);
    }
}

/**
 * Parses a byte sequence and reconstructs into an integer of type T, converting from
 * Network Byte Order/Big Endian to Host Byte Order. The byte sequence must have at
 * least sizeof(T) bytes.
 */
template<typename T, typename InputIt>
constexpr T parse_be(InputIt it) noexcept
{
    T h = 0;
    for(auto i = 0; i < int(sizeof h); ++i)
    {
        h <<= 8;
        h |= static_cast<uint8_t>(*it++);
    }
    return h;
}

/**
 * Parses a byte sequence and reconstructs into an integer of type T, converting from
 * Little Endian to Host Byte Order. The byte sequence must have at least sizeof(T)
 * bytes.
 */
template<typename T, typename InputIt>
constexpr T parse_le(InputIt it) noexcept
{
    T h = 0;
    for(auto i = 0; i < int(sizeof h); ++i)
    {
        h |= static_cast<uint8_t>(*it++) << i * 8;
    }
    return h;
}

} // namespace detail
} // namespace atag

#endif // ATAG_IO_UTIL_HEADER
