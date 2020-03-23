#ifndef ATAG_ID3_IMPL_HEADER
#define ATAG_ID3_IMPL_HEADER

#include "../detail/type_traits.hpp"
#include "../detail/io_util.hpp"
#include "../encoding.hpp"
#include "../id3v2.hpp"

#include <cassert>
#include <algorithm>
#include <iterator>
#include <array>
#include <locale>
#include <codecvt>
#ifdef ATAG_ENABLE_DEBUGGING
# include <cstdio>
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
#endif // ATAG_ENABLE_DEBUGGING

namespace atag {
namespace id3v2 {

struct tag_header
{
    char version;
    char revision;
    unsigned char flags;
    int size;
    int extended_header_size;
};

struct frame_header
{
    char id;
    uint16_t flags;
    int size;
};

/** `s` must be a buffer or a pointer to a buffer starting at the tag header. */
template<typename Ptr>
tag_header parse_tag_header(Ptr s) noexcept
{
    tag_header h;
    // We can skip the "ID3" identifier as find_tag_start implicitly verifies its presence.
    h.version = s[3];
    h.revision = s[4];
    h.flags = s[5];
    h.size = detail::parse_syncsafe<int>(&s[6]);
#ifdef ATAG_ENABLE_DEBUGGING
    std::printf("tag header:: magic: ID3, version: %i, revision: %i, size: %i, flags: ("
        ATAG_BYTE_BINARY_PATTERN")\n", h.version, h.revision, h.size,
        ATAG_BYTE_TO_BINARY(h.flags));
#endif // ATAG_ENABLE_DEBUGGING
    if(h.flags & tag::flags::extended)
        h.extended_header_size = detail::parse_syncsafe<int>(&s[10]);
    else
        h.extended_header_size = 0;
    return h;
}

/** `s` must be a buffer or a pointer to a buffer starting at the frame header. */
template<typename Ptr>
frame_header parse_frame_header(Ptr s) noexcept
{
    frame_header h;
    h.id = frame_id_from_string(s);
    h.flags = 0;
    h.flags |= s[8] << 8;
    h.flags |= s[9];
    h.size = detail::parse_syncsafe<int>(&s[4]);
#ifdef ATAG_ENABLE_DEBUGGING
    if((h.id != -1) && (h.size > 0))
        std::printf("frame header:: id: %s(%s), size: %i, flags: ("
            ATAG_BYTE_BINARY_PATTERN " " ATAG_BYTE_BINARY_PATTERN")\n",
            frame_id_to_string(h.id), frame_id_to_hrstring(h.id), h.size,
            ATAG_BYTE_TO_BINARY(h.flags << 8), ATAG_BYTE_TO_BINARY(h.flags));
#endif // ATAG_ENABLE_DEBUGGING
    return h;
}

/** `s` must be a buffer or a pointer to a buffer starting at the frame body. */
template<typename Ptr>
tag::frame parse_frame_body(const frame_header& header, Ptr s)
{
    // TODO FIXME we'll need a lot more branching here depending on the frame's type
    tag::frame frame;
    frame.id = header.id;
    frame.flags = header.flags;
    // TODO we need to find all frames that have encoding and apply the same conv (as
    // frames other than text frames may have text in them, such as `comments`).
    if(is_text_frame(header.id) || (frame.id == comments))
    {
        // Textual information.
        frame.encoding = s[0];
#ifdef ATAG_ENABLE_DEBUGGING
        std::printf("\ttext frame encoding: %s\n", frame.encoding == iso_8859_1
                ? "ISO-8859-1" : frame.encoding == utf16
                    ? "UTF-16" : frame.encoding == utf16be
                        ? "UTF-16BE" : "UTF8");
#endif // ATAG_ENABLE_DEBUGGING
        switch(frame.encoding) {
        case encoding::iso_8859_1:
            frame.data = atag::encoding::iso_8859_1_to_utf8(s + 1, header.size - 1);
            //frame.data = std::string(s + 1, header.size - 1);
            break;
        case encoding::utf16:
        {
            // header.size is the number of bytes while UTF-16 is at least 2 bytes wide.
            // Also, we must start at the fourth byte, since the first is the encoding
            // identifier, the second and third the BOM, which the spec requires.
            frame.data = atag::encoding::utf16le_to_utf8(
                reinterpret_cast<const char16_t*>(s + 3), (header.size >> 1) - 1);
            break;
        }
        case encoding::utf16be:
        {
            frame.data = atag::encoding::utf16be_to_utf8(
                reinterpret_cast<const char16_t*>(s + 3), (header.size >> 1) - 1);
            break;
        }
        case encoding::utf8:
            frame.data = std::string(s + 1, header.size - 1);
            break;
        }
    }
    else
    {
        frame.data = std::string(s, header.size);
    }
    return frame;
}

inline bool is_frame_header_valid(const frame_header& header) noexcept
{
    // TODO
    return (header.id != char(-1)) && (header.size > 0);
}

template<typename Source>
inline int find_tag_start(const Source& s) noexcept
{
    const auto matches = [&s](const auto* p, const char* magic)
    {
        return std::equal(p, p + 3, magic)
            && (p[3] <= 4) && (p[4] < 0xff)
            && (p[6] < 0x80) && (p[7] < 0x80) && (p[8] < 0x80) && (p[9] < 0x80);
    };

    // Most ID3v2 tags will be prepended to the file, so start with that.
    if(matches(&s[0], "ID3")) { return 0; }

    // See if the tag is appended (in which case it must have a footer at the very end
    // of the file).
    const int n = s.size();
    if(matches(&s[n-10], "3DI"))
    {
        const auto tag_size = detail::parse_syncsafe<int>(&s[n-4]);
        // Since there is a header and a footer, subtract the 10 byte header size twice.
        return n - tag_size - 20;
    }

    // No tag could be found.
    return -1;
}

template<typename Source>
bool is_tagged(const Source& s) noexcept
{
    //static_assert(detail::is_source<Source>::value, "Source requirements not met");
    return find_tag_start(s) != -1;
}

template<typename Source>
tag parse(const Source& s)
{
    return parse(s, [](int _) { return true; });
}

template<typename Source>
tag parse(const Source& s, const std::initializer_list<int>& wanted_frames)
{
    return parse(s, [&wanted_frames](const int id)
        {
            return std::find(wanted_frames.begin(), wanted_frames.end(), id)
                    != wanted_frames.end();
        });
}

template<typename Source, typename Predicate>
tag parse(const Source& s, Predicate pred)
{
    static_assert(detail::is_source<Source>::value, "Source requirements not met");

    // TODO consider using std::error_code instead
    //TODO:M.P:(s.size() < 10) { throw "source must be at least 10 bytes long"; }

    const int tag_start = find_tag_start(s);
    if(tag_start == -1) { return {}; }

    const auto tag_header = parse_tag_header(&s[tag_start]);
    tag tag;
    tag.version = tag_header.version;
    tag.revision = tag_header.revision;
    tag.flags = tag_header.flags;
    // Now parse the frames, starting after the header + extended header.
    for(auto i = tag_start + 10 + tag_header.extended_header_size,
             n = tag_start + std::min(int(s.size()), tag_header.size); i < n;)
    {
        const auto frame_header = parse_frame_header(&s[i]);
        if(is_frame_header_valid(frame_header) && pred(frame_header.id))
        {
            tag.frames.emplace_back(parse_frame_body(frame_header, &s[i+10]));
#ifdef ATAG_ENABLE_DEBUGGING
            std::printf("frame body:: %s\n", tag.frames.back().data.c_str());
#endif // ATAG_ENABLE_DEBUGGING
        }
        i += frame_header.size + 10;
    }
    return tag;
}

/** `s` must be a buffer or a pointer to a buffer starting at the frame body. */
template<typename Source>
void simple_parse_dispatch(const Source& s,
    const frame_header& header, simple_tag& tag)
{
    switch(header.id) {
    case hrid::title: case hrid::original_title:
        if(tag.title.empty())
            tag.title = std::move(parse_frame_body(header, s).data);
        break;
    case hrid::album:
        if(tag.album.empty())
            tag.album = std::move(parse_frame_body(header, s).data);
        break;
    case hrid::lead_artist: case hrid::composer: case hrid::original_performer:
        if(tag.artist.empty())
            tag.artist = std::move(parse_frame_body(header, s).data);
        break;
    case hrid::year:
        tag.year = std::atoi(&s[1]);
        break;
    case hrid::track_number:
        tag.track_number = std::atoi(&s[1]);
        break;
    case hrid::length:
        tag.length = std::atoi(&s[1]);
        break;
    }
}

template<typename Source>
simple_tag simple_parse(const Source& s)
{
    //static_assert(detail::is_source<Source>::value, "Source requirements not met");

    //TODO:M.P:if(s.size() < 10) { throw "source must be at least 10 bytes long"; }

    const int tag_start = find_tag_start(s);
    if(tag_start == -1) { return {}; }

    const auto tag_header = parse_tag_header(&s[tag_start]);
    simple_tag tag;
    // Now parse the frames, starting after the header + extended header.
    for(auto i = tag_start + 10 + tag_header.extended_header_size; i < tag_header.size;)
    {
        const auto frame_header = parse_frame_header(&s[i]);
        if(is_frame_header_valid(frame_header))
        {
            simple_parse_dispatch(&s[i+10], frame_header, tag);
        }
        i += frame_header.size + 10;
    }
    return tag;
}

// NOTE: this _must_ be kept in the same order as the tag::frame::id values.
constexpr static const struct {
    int id;
    const char* raw;
    const char* human_readable;
} frame_ids_[] = {
    {tag::frame::aenc, "AENC", "audio encryption"},
    {tag::frame::apic, "APIC", "attached picture"},
    {tag::frame::aspi, "ASPI", "audio seek point index"},
    {tag::frame::comm, "COMM", "comments"},
    {tag::frame::comr, "COMR", "commercial frame"},
    {tag::frame::encr, "ENCR", "encryption method registration"},
    {tag::frame::equa, "EQUA", "equalisation"},
    {tag::frame::etco, "ETCO", "event timing codes"},
    {tag::frame::geob, "GEOB", "general encapsulated object"},
    {tag::frame::grid, "GRID", "group identification registration"},
    {tag::frame::ipls, "IPLS", "involved people list"},
    {tag::frame::link, "LINK", "linked information"},
    {tag::frame::mcdi, "MCDI", "music CD identifier"},
    {tag::frame::mllt, "MLLT", "MPEG location lookup table"},
    {tag::frame::owne, "OWNE", "ownership frame"},
    {tag::frame::pcnt, "PCNT", "play counter"},
    {tag::frame::popm, "POPM", "popularimeter"},
    {tag::frame::poss, "POSS", "position synchronisation frame"},
    {tag::frame::priv, "PRIV", "private frame"},
    {tag::frame::rbuf, "RBUF", "recommended buffer size"},
    {tag::frame::rvad, "RVAD", "relative volume adjustment"},
    {tag::frame::rvrb, "RVRB", "reverb"},
    {tag::frame::seek, "SEEK", "seek frame"},
    {tag::frame::sign, "SIGN", "signature frame"},
    {tag::frame::sylt, "SYLT", "synchronised lyric/text"},
    {tag::frame::sytc, "SYTC", "synchronized tempo codes"},

    {tag::frame::talb, "TALB", "album"},
    {tag::frame::tbpm, "TBPM", "beats/minute"},
    {tag::frame::tcom, "TCOM", "composer"},
    {tag::frame::tcon, "TCON", "content type"},
    {tag::frame::tcop, "TCOP", "copyright"},
    {tag::frame::tdat, "TDAT", "date"},
    {tag::frame::tden, "TDEN", "encoding time"},
    {tag::frame::tdly, "TDLY", "playlist delay"},
    {tag::frame::tenc, "TENC", "encoded by"},
    {tag::frame::text, "TEXT", "lyricist(s)"},
    {tag::frame::tflt, "TFLT", "file type"},
    {tag::frame::time, "TIME", "time"},
    {tag::frame::tit1, "TIT1", "content group"},
    {tag::frame::tit2, "TIT2", "title"},
    {tag::frame::tit3, "TIT3", "subtitle"},
    {tag::frame::tkey, "TKEY", "initial key"},
    {tag::frame::tlan, "TLAN", "language(s)"},
    {tag::frame::tlen, "TLEN", "length (ms)"}, 
    {tag::frame::tmed, "TMED", "media type"},
    {tag::frame::toal, "TOAL", "original title"},
    {tag::frame::tofn, "TOFN", "original filename"},
    {tag::frame::toly, "TOLY", "original lyricist(s)"},
    {tag::frame::tope, "TOPE", "original performer"},
    {tag::frame::tory, "TORY", "original release year"},
    {tag::frame::town, "TOWN", "licensee"},
    {tag::frame::tpe1, "TPE1", "lead artist(s)"},
    {tag::frame::tpe2, "TPE2", "band/orchestra/accompaniment"},
    {tag::frame::tpe3, "TPE3", "conductor/performer"},
    {tag::frame::tpe4, "TPE4", "interpreted/remixed/modified by"},
    {tag::frame::tpos, "TPOS", "part of a set"},
    {tag::frame::tpub, "TPUB", "publisher"},
    {tag::frame::trck, "TRCK", "track number"},
    {tag::frame::trda, "TRDA", "recording dates"},
    {tag::frame::trsn, "TRSN", "internet radio station name"},
    {tag::frame::trso, "TRSO", "internet radio station owner"},
    {tag::frame::tsiz, "TSIZ", "size (bytes)"},
    {tag::frame::tsrc, "TSRC", "ISRC"},
    {tag::frame::tsse, "TSSE", "encoding software/hardware settings"},
    {tag::frame::txxx, "TXXX", "user defined text"},
    {tag::frame::tyer, "TYER", "year"},

    {tag::frame::ufid, "UFID", "unique file identifier"},
    {tag::frame::user, "USER", "terms of use"},
    {tag::frame::uslt, "USLT", "unsynchronized lyrics"},

    {tag::frame::wcom, "WCOM", "commercial information"},
    {tag::frame::wcop, "WCOP", "copyright information"},
    {tag::frame::woaf, "WOAF", "offical audio file webpage"},
    {tag::frame::woar, "WOAR", "offical artist webpage"},
    {tag::frame::woas, "WOAS", "official audio source webpage"},
    {tag::frame::wors, "WORS", "official internet radio station homepage"},
    {tag::frame::wpay, "WPAY", "payment"},
    {tag::frame::wpub, "WPUB", "publisher official webpage"},
    {tag::frame::wxxx, "WXXX", "user defined link"}
};

template<typename String>
int frame_id_from_string(const String& s) noexcept
{
    const auto it = std::find_if(std::begin(frame_ids_), std::end(frame_ids_),
        [&s](const auto& f) { return std::equal(f.raw, f.raw + 4, &s[0]); });
    if(it != std::end(frame_ids_))
        return it->id;
    else
        return -1;
}

constexpr const char* frame_id_to_string(const int id) noexcept
{
    if((id >= int(tag::frame::aenc)) && (id <= int(tag::frame::wxxx)))
        return frame_ids_[id].raw; 
    else
        return nullptr;
}

constexpr const char* frame_id_to_hrstring(const int id) noexcept
{
    if((id >= int(tag::frame::aenc)) && (id <= int(tag::frame::wxxx)))
        return frame_ids_[id].human_readable; 
    else
        return nullptr;
}

} // namespace id3v2
} // namespace atag

#endif // ATAG_ID3_IMPL_HEADER
