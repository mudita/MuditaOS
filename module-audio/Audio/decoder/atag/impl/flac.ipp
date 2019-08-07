#ifndef ATAG_FLAC_IMPL_HEADER
#define ATAG_FLAC_IMPL_HEADER

#include "../detail/type_traits.hpp"
#include "../detail/io_util.hpp"
#include "../flac.hpp"

#include <algorithm>
#include <array>
#ifdef ATAG_ENABLE_DEBUGGING
# include <cstdlib>
# define ATAG_BYTE_BINARY_PATTERN "%c%c%c%c %c%c%c%c"
# define ATAG_BYTE_TO_BINARY(byte) \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')
#endif

namespace atag {
namespace flac {

struct block_header
{
    enum type
    {
        streaminfo,
        padding,
        application,
        seektable,
        vorbis_comment,
        cuesheet,
        picture
    };

    enum
    {
        min_size = 16,
        max_size = 65535
    };

    bool is_last_block;
    enum type type;
    int length;
};

/** Tests whether `s` contains a FLAC tag. */
template<typename Source>
bool is_tagged(const Source& s) noexcept
{
    static_assert(detail::is_source<Source>::value, "Source requirements not met");
    return (s.size() >= 5) && std::equal(&s[0], &s[4], "fLaC");
}

/* `s` must be a buffer or pointer to a buffer starting at the block header. */
template<typename Byte>
block_header parse_block_header(const Byte* s)
{
    block_header h;
    h.is_last_block = s[0] & 0b1000'0000;
    h.type = static_cast<enum block_header::type>(s[0] & 0b0111'1111);
    h.length = detail::parse_be<int>(&s[0]);
    // Only the last 3 bytes are used for the length field.
    h.length &= 0b0000'0000'1111'1111'1111'1111;
#ifdef ATAG_ENABLE_DEBUGGING
    std::printf("block header:: is_last_block: %d, type: %i, length: %i\n",
        h.is_last_block, h.type, h.length);
#endif // ATAG_ENABLE_DEBUGGING
    return h;
}

/* `s` must be a buffer or pointer to a buffer starting at the streaminfo block's data. */
template<typename Byte>
void parse_streaminfo(const Byte* s, tag& tag)
{
    // sample_rate is stored in 2.5 bytes/20 bits
    tag.sample_rate = detail::parse_be<int>(&s[10]);
    tag.sample_rate >>= 12;
    // num_channels is stored in 3 bits
    tag.num_channels = (s[12] & 0b0000'1110) >> 1;
    // total samples is stored in 5 bits, with 1 bit in another byte
    tag.num_samples = 0;
    tag.num_samples |= (s[12] & 0b0000'0001) << 4; 
    tag.num_samples |= (s[13] & 0b1111'0000) >> 4; 
}

/**
 * `s` must be a buffer or pointer to a buffer starting at the vorbis comment
 * block's data.
 */
template<typename Byte>
void parse_vorbis_comment(const Byte* s, tag& tag)
{
    // TODO refactor
    const int vendor_length = detail::parse_le<uint32_t>(&s[0]);
#ifdef ATAG_ENABLE_DEBUGGING
    if(vendor_length > 0)
    {
        const std::string vendor_string(&s[4], vendor_length);
        std::printf("vendor string: %s (%i bytes)\n",
            vendor_string.c_str(), vendor_length);
    }
#endif
    int offset = vendor_length + 4;
    const int comment_list_size = detail::parse_le<uint32_t>(&s[offset]);
    offset += 4;
    // TODO also make sure we don't iterate out of s
    for(auto i = 0; i < comment_list_size; ++i)
    {
        const uint32_t comment_length = detail::parse_le<uint32_t>(&s[offset]);
        offset += 4;
        const auto comment_begin = &s[offset];
        const auto comment_end = comment_begin + comment_length;
        const auto sep_pos = std::find(comment_begin, comment_end, '=');
        // There must always be a '=' separator between the comment's key and value
        // TODO or must there?
        if(sep_pos == comment_end) { break; }

        const auto value_begin = sep_pos + 1;
        const int key_length = sep_pos - comment_begin;
        const int value_length = comment_end - value_begin;
        switch(key_length) {
#define KEY_EQUALS(s) std::equal(comment_begin, comment_begin + sizeof(s) - 1, s)
        case 4:
            // Only parse date if it's 4 chars long representing the year.
            // TODO more flexibility
            if(KEY_EQUALS("DATE") && (value_length == 4))
                tag.year = std::atoi(value_begin);
            break;
        case 5:
            if(KEY_EQUALS("ALBUM"))
                tag.album = std::string(value_begin, value_length);
            else if(KEY_EQUALS("GENRE"))
                tag.genre = std::string(value_begin, value_length);
            else if(KEY_EQUALS("TITLE"))
                tag.title = std::string(value_begin, value_length);
            break;
        case 6:
            if(KEY_EQUALS("ARTIST"))
            {
                // Vorbis allows multiple comments with the same key.
                if(tag.artist.empty())
                    tag.artist = std::string(value_begin, value_length);
                else
                    tag.artist += ", " + std::string(value_begin, value_length);
            }
            break;
        case 9:
            // TODO does it make sense to treat 'performer' as 'artist'?
            if(KEY_EQUALS("PERFORMER"))
            {
                if(tag.artist.empty())
                    tag.artist = std::string(value_begin, value_length);
                else
                    tag.artist += ", " + std::string(value_begin, value_length);
            }
            break;
        case 11:
            if(KEY_EQUALS("TRACKNUMBER"))
                tag.track_number = std::atoi(value_begin);
            break;
#undef KEY_EQUALS
        }
        offset += comment_length;
    }
}

/** Parses and extracts all frames found in `s`. */
template<typename Source>
tag parse(const Source& s)
{
    static_assert(detail::is_source<Source>::value, "Source requirements not met");

    if(!is_tagged(s)) { return {}; }

    // https://xiph.org/flac/format.html#metadata_block
    tag tag;
    // Skip ahead by 4 to skip the 4 byte FLAC tag ID.
    for(auto i = 4; i + 4 < int(s.size());)
    {
        const auto block_header = parse_block_header(&s[i]);
        if(block_header.length <= 0)
        {
#ifdef ATAG_ENABLE_DEBUGGING
            std::printf("invalid block length: %i\n", block_header.length);
            std::printf(ATAG_BYTE_BINARY_PATTERN"  " ATAG_BYTE_BINARY_PATTERN"  "
                   ATAG_BYTE_BINARY_PATTERN"  " ATAG_BYTE_BINARY_PATTERN"\n",
                   ATAG_BYTE_TO_BINARY(s[i]), ATAG_BYTE_TO_BINARY(s[i+1]),
                   ATAG_BYTE_TO_BINARY(s[i+2]), ATAG_BYTE_TO_BINARY(s[i+3]));
#endif
            break;
        }

#ifdef ATAG_ENABLE_DEBUGGING
# define ATAG_FLAC_BLOCK(block_name) do std::printf(block_name " block\n"); while(0)
#else
# define ATAG_FLAC_BLOCK(block_name) (void)0
#endif
        // Advance past the block header to the body.
        i += 4;
        switch(block_header.type) {
        case block_header::type::streaminfo:
            ATAG_FLAC_BLOCK("streaminfo");
            parse_streaminfo(&s[i], tag);
            break;
        case block_header::type::padding:
            ATAG_FLAC_BLOCK("padding");
            break;
        case block_header::type::application:
            ATAG_FLAC_BLOCK("application");
            break;
        case block_header::type::seektable:
            ATAG_FLAC_BLOCK("seektable");
            break;
        case block_header::type::vorbis_comment:
            ATAG_FLAC_BLOCK("vorbis comment");
            parse_vorbis_comment(&s[i], tag);
            break;
        case block_header::type::cuesheet:
            ATAG_FLAC_BLOCK("cuesheet");
            break;
        case block_header::type::picture:
            ATAG_FLAC_BLOCK("picture");
            break;
        default:
            ATAG_FLAC_BLOCK("unknown");
            return tag;
        }

        if(block_header.is_last_block)
            break;
        else
            i += block_header.length;
    }
    return tag;
}

} // namespace flac
} // namespace atag

#endif // ATAG_FLAC_IMPL_HEADER
